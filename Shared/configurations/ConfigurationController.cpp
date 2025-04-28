/*******************************************************************************
 *  Copyright 2012-2025 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

#include "ConfigurationController.h"

// Qt headers
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QtGlobal>
#include <QUuid>

// DSA headers
#include "Configuration.h"
#include "ConfigurationListModel.h"
#include "DsaUtility.h"

namespace Dsa {

ConfigurationController::ConfigurationController(QObject* parent /* = nullptr */):
  AbstractTool(parent),
  m_configurationListModel(new ConfigurationListModel{this})
{
  connect(m_configurationListModel, &ConfigurationListModel::dataChanged, this, [this]() { emit configurationsChanged(); });

  ToolManager::instance().addTool(this);

  m_networkAccessManager.setAutoDeleteReplies(true);

  // set the downloads folder based on the system type
#ifdef Q_OS_IOS
  m_downloadFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  m_downloadFolder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#endif

  // populate the configurations from disk
  fetchConfigurations();
}

void ConfigurationController::select(int index)
{
  m_configurationListModel->select(index);
  storeConfigurations();
  emit configurationsChanged();
}

void ConfigurationController::download(int index)
{
  // prevent calling the download function if something is already in progress
  if (m_downloadInProgress)
    return;

  // get the url from the selected item
  auto i = m_configurationListModel->index(index, 0);
  auto url = m_configurationListModel->data(i, ConfigurationListModel::Roles::Url);
  m_activeDownloadIndex = index;

  // generate a unique name for the download
  QDir downloadFolder(m_downloadFolder);
  m_downloadFileName = downloadFolder.absoluteFilePath((QString("dsa_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces))));
  QNetworkRequest networkRequest{QUrl{url.toString()}};

  // make a head request to check the availabiity of the network as well as get the final size of the package to be downloaded
  m_aborted = false;
  auto* headReply = m_networkAccessManager.head(networkRequest);
  connect(headReply, &QNetworkReply::finished, this, [this, headReply, networkRequest]()
  {
    // get the total bytes that will be downloaded
    bool convertedOk = false;
    int bytesToDownload = 0;
    auto contentLengthVariant = headReply->header(QNetworkRequest::ContentLengthHeader);
    if (contentLengthVariant.isValid())
      bytesToDownload = contentLengthVariant.toInt(&convertedOk);

    // abort if the download size could not be fetched
    if (!convertedOk || bytesToDownload <= 0)
    {
      emit toolErrorOccurred("Unable to fetch the download size.", "Network Error");
      return;
    }

    // make sure the device has enough room for the download
    QStorageInfo storageInfo(m_downloadFolder);
    auto bytesAvailable = storageInfo.bytesAvailable() * 0.95;
    if (bytesToDownload > bytesAvailable)
    {
      emit toolErrorOccurred("The total download size exceeds storage available on the device", "Download Error");
      return;
    }

    // reset the properties for download status and use the network manager to download the file
    m_networkReply = m_networkAccessManager.get(networkRequest);
    m_downloadInProgress = true;
    connect(m_networkReply, &QNetworkReply::downloadProgress, this, &ConfigurationController::downloadProgress);
    connect(m_networkReply, &QNetworkReply::readyRead, this, &ConfigurationController::readyRead);
    connect(m_networkReply, &QNetworkReply::finished, this, &ConfigurationController::finished);
    connect(m_networkReply, &QNetworkReply::errorOccurred, this, &ConfigurationController::downloadErrorOccurred);
  });
}

void ConfigurationController::cancel(int index)
{
  Q_UNUSED(index); // TODO: supporting multiple downloads at a time will require additional management
  if (m_networkReply && m_networkReply->isOpen())
  {
    // cancel the downloading item and reset all the
    m_configurationListModel->cancel(index);
    m_aborted = true;
    m_downloadInProgress = false;
    m_networkReply->abort();
  }
}

void ConfigurationController::remove(int index)
{
  // make extracted packages removeable from device
  Q_UNUSED(index);
}

void ConfigurationController::storeConfigurations()
{
  QJsonArray array;
  for (const auto& cfg : *m_configurationListModel)
  {
    QJsonObject obj;
    obj["selected"] = cfg.selected();
    obj["name"] = cfg.name();
    obj["url"] = cfg.url();
    array.append(obj);
  }
  const QJsonDocument doc{array};
  QFile file{DsaUtility::configurationsFilePath()};
  if (file.open(QIODevice::WriteOnly))
    file.write(doc.toJson());
}

bool ConfigurationController::createDefaultConfigurationsFile()
{
  // create a single entry for the default configuration
  QJsonObject obj;
  obj["selected"] = true;
  obj["name"] = DEFAULT_DOWNLOAD_NAME;
  obj["url"] = DEFAULT_DOWNLOAD_URL;

  // create a new document as a top level array and append the default object
  QJsonArray array;
  array.append(obj);
  QJsonDocument doc{array};

  // abort if unable to open the configurations file for writing
  QFile file{DsaUtility::configurationsFilePath()};
  if (!file.open(QIODevice::WriteOnly))
    return false;

  // write out the document to disk
  file.write(doc.toJson());
  return true;
}

QString ConfigurationController::toolName() const
{
  return QStringLiteral("configurations");
}

bool ConfigurationController::downloadInProgress()
{
  return m_downloadInProgress;
}

bool ConfigurationController::requiresRestart()
{
  // check the configurations for reload
  for (const auto& cfg : *m_configurationListModel)
  {
    if (cfg.requiresRestart())
      return true;
  }
  return false;
}

bool ConfigurationController::configurationIsAvailable()
{
  // look for a downloaded configuration
  for (const auto& cfg : *m_configurationListModel)
  {
    if (cfg.downloaded())
      return true;
  }
  return false;
}

QAbstractListModel* ConfigurationController::configurations() const
{
  return m_configurationListModel;
}

void ConfigurationController::downloadProgress(quint64 bytesReceived, quint64 bytesTotal)
{
  // guard against dividing by zero
  if (bytesTotal == 0)
    return;

  setPercentComplete(bytesReceived * 1.0 / bytesTotal * 1.0 * 100);
}

void ConfigurationController::readyRead()
{
  // make sure the download file can be opened for appending
  QFile file(m_downloadFileName);
  if (!file.open(QIODevice::Append))
  {
    m_aborted = true;
    return;
  }

  // append the latest chunk to the download file in progress
  file.write(m_networkReply->readAll());
}

void ConfigurationController::finished()
{
  // turn off the downloading state and clean up any aborted download files remaining
  m_downloadInProgress = false;
  if (m_aborted)
  {
    QFile fileAbort(m_downloadFileName);
    if (fileAbort.exists())
      fileAbort.remove();

    // reset the abort state and percent complete
    m_aborted = false;
    setPercentComplete(0);
    return;
  }

  // scope block to cleanup qfile to finish the writing of the stream
  {
    QFile fileFinish{m_downloadFileName};
    if (!fileFinish.open(QIODevice::Append))
    {
      emit toolErrorOccurred("Unable to complete download", "Error Downloading");
      return;
    }

    fileFinish.write(m_networkReply->readAll());
  }
  setPercentComplete(100);

  // initialize the zip helper with the path to the downloaded file
  m_zipHelper = new ZipHelper(m_downloadFileName, this);
  connect(m_zipHelper, &ZipHelper::extractCompleted, this, &ConfigurationController::extractCompleted);
  connect(m_zipHelper, &ZipHelper::extractError, this, &ConfigurationController::extractError);

  // resolve the download directory from the active item index
  auto activeDownloadIndex = m_configurationListModel->index(m_activeDownloadIndex);
  auto activeDownloadName = m_configurationListModel->data(activeDownloadIndex, ConfigurationListModel::Roles::Name).toString();
  QDir dirConfigurations{DsaUtility::configurationsDirectoryPath()};
  QDir dirActiveDownloadConfiguration{dirConfigurations.filePath(activeDownloadName)};
  if (!dirActiveDownloadConfiguration.exists())
    dirConfigurations.mkdir(activeDownloadName);

  // extract the downloaded file to the configuration folder
  m_zipHelper->extractAll(dirActiveDownloadConfiguration.absolutePath());
}

void ConfigurationController::downloadErrorOccurred(QNetworkReply::NetworkError error)
{
  m_aborted = true;
  if (error != QNetworkReply::NetworkError::OperationCanceledError)
    emit toolErrorOccurred(QString("Network Error (%1)").arg(error), "Error Downloading");
}

void ConfigurationController::extractCompleted()
{
  // delete the temporary download from the device
  QFile::remove(m_downloadFileName);
  setPercentComplete(100);
  m_zipHelper->deleteLater();
  m_zipHelper = nullptr;
}

void ConfigurationController::extractError(const QString& fileName, const QString& outputFileName, ZipHelper::Result result)
{
  Q_UNUSED(result);
  emit toolErrorOccurred(QString("error %1, %2").arg(fileName, outputFileName), "Error Unzipping");
  QFile::remove(m_downloadFileName);
  setPercentComplete(0);
  m_zipHelper->deleteLater();
  m_zipHelper = nullptr;
}

void ConfigurationController::downloadDefaultData()
{
  qsizetype i = 0;
  for (const auto& cfg : *m_configurationListModel)
  {
    if (cfg.name() == DEFAULT_DOWNLOAD_NAME)
    {
      download(i);
      return;
    }
    i++;
  }
}

void ConfigurationController::setPercentComplete(int percentComplete)
{
  auto i = m_configurationListModel->index(m_activeDownloadIndex, 0);
  m_configurationListModel->setData(i, percentComplete, ConfigurationListModel::Roles::PercentDownloaded);
  emit configurationsChanged();
}

void ConfigurationController::fetchConfigurations()
{
  // check for the default configurations file and create if it doesn't exist
  m_configurationListModel->removeRows(0, m_configurationListModel->rowCount(QModelIndex{}));
  QFile fileConfigurations{DsaUtility::configurationsFilePath()};
  QDir dirConfigurations{DsaUtility::configurationsDirectoryPath()};

  // abort if io failure and file couldnt be opened
  if (!fileConfigurations.open(QIODevice::ReadOnly))
    return;

  // configurations file must not be empty or something other than a top-level json array
  QJsonParseError parseError;
  const auto docConfigurations = QJsonDocument::fromJson(fileConfigurations.readAll(), &parseError);
  bool alreadySelected = false;
  if (docConfigurations.isNull() || docConfigurations.isEmpty() || !docConfigurations.isArray())
    return;

  // inspect each node from the array
  const auto configurationsArray = docConfigurations.array();
  for (const auto& configurationNode : configurationsArray)
  {
    // skip any nodes that are not object types
    if (!configurationNode.isObject())
      continue;

    // add a new item
    const auto configurationObject = configurationNode.toObject();

    // determine if the configuration has been downloaded by checking for the configuration file and at least
    // some other files in the directory
    bool downloaded = false;
    auto configurationName = configurationObject["name"].toString();
    auto configurationDirectoryPath = dirConfigurations.filePath(configurationName);
    QDir dirConfiguration{configurationDirectoryPath};

    // skip directories that are not found on disk
    if (!dirConfiguration.exists())
      continue;

    // skip if the app config cannot be found in the directory
    if (!QFile::exists(dirConfiguration.filePath(DsaUtility::FILE_NAME_APP_CONFIG)))
      continue;

    // check the folder recursively for other files
    QDirIterator dirIt(configurationDirectoryPath, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    quint8 filesInConfigurationFolder = 0;
    while (dirIt.hasNext())
    {
      dirIt.next();
      filesInConfigurationFolder++;
      if (filesInConfigurationFolder > 1) // download is valid if more than one file is found
      {
        downloaded = true;
        break;
      }
    }

    // select the first item marked as selected
    bool selected = false;
    if (!alreadySelected)
    {
      selected = configurationObject["selected"].toBool();
      alreadySelected = selected;
    }

    // add the new entry
    m_configurationListModel->add(configurationName,
                                  configurationObject["url"].toString(),
                                  selected,
                                  selected && downloaded,
                                  downloaded ? 100 : 0);
  }
  emit configurationsChanged();
}

}
