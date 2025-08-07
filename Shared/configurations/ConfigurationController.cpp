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
#include <QFuture>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QtConcurrentRun>
#include <QtGlobal>
#include <QUuid>

// C++ API headers
#include "Error.h"
#include "ErrorException.h"
#include "NetworkRequestProgress.h"
#include "Portal.h"
#include "PortalItem.h"

// DSA headers
#include "AppConstants.h"
#include "Configuration.h"
#include "ConfigurationListModel.h"
#include "DsaUtility.h"
#include "ZipHelper.h"

using namespace Esri::ArcGISRuntime;

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
  m_configurationsDirectory = QDir(DsaUtility::configurationsDirectoryPath());
  fetchConfigurations();
}

void ConfigurationController::select(int index)
{
  m_configurationListModel->select(index);
  storeConfigurations();
  emit configurationsChanged();
}

bool removeDownloadedFile(const QString& downloadFilePath)
{
  // remove any remaining progress if request was aborted or after the zip has been extracted
  QFile f{downloadFilePath};
  if (f.exists())
    return f.remove();

  return false;
}

void ConfigurationController::extractConfigurationDownload(const QString& downloadFilePath, const QString& configurationName)
{
  // create the folder to unpack the zip contents to
  const QDir configurationDirectory{m_configurationsDirectory.filePath(configurationName)};
  if (!configurationDirectory.exists())
    m_configurationsDirectory.mkdir(configurationName);

  // initialize the zip helper with the path to the downloaded file
  auto* zipHelper = new ZipHelper(downloadFilePath, this);
  connect(zipHelper, &ZipHelper::extractCompleted, this, [this, configurationName, configurationDirectory, zipHelper, downloadFilePath]()
  {
    zipHelper->deleteLater();

    // the zip file needs to be cleaned up from the downloads folder for anything
    // other than a local file. don't delete local files becasue they are not copied
    // and the extract is done pointing directly to the file url (file://...)
    const auto configuration = m_configurationListModel->byName(configurationName);
    if (!configuration.url().isLocalFile())
      removeDownloadedFile(downloadFilePath);

    // we can exit early if the update of the DsaAppConfig.json file had no issues
    if (updateExtractedConfigurationFile(configurationDirectory))
      return;

    // alert the user if the package was anything other than the default from esri which
    // is expected to fail because it contains no DsaAppConfig.json file
    if (configuration.urlStr() != ConfigurationController::DEFAULT_DOWNLOAD_URL)
      emit toolErrorOccurred(QStringLiteral("The DsaAppConfig.json file could not be updated. The application will create a default version for the configuration."), QStringLiteral("Configuration Error"));
  });
  connect(zipHelper, &ZipHelper::extractProgressTotal, this, [this, configurationName](qsizetype percentTotal)
  {
    m_configurationListModel->setDataByName(configurationName, percentTotal, ConfigurationListModel::PercentExtracted);
  });
  connect(zipHelper, &ZipHelper::extractError, this, [this, configurationName, zipHelper, downloadFilePath](const QString& fileName, const QString& outputFileName, ZipHelper::Result result)
  {
    Q_UNUSED(result);
    Q_UNUSED(fileName);
    Q_UNUSED(outputFileName);
    zipHelper->deleteLater();
    removeDownloadedFile(downloadFilePath);
    emit toolErrorOccurred(QStringLiteral("Error unzipping the download"), QStringLiteral("Configuration Error"));

    resetConfigurationDeviceStatus(configurationName);
  });

  // extract the downloaded file to the configuration
  const auto extractFuture = QtConcurrent::run([configurationDirectory, zipHelper]
  {
    zipHelper->extractAll(configurationDirectory.absolutePath());
  });
  Q_UNUSED(extractFuture);
}

bool ConfigurationController::updateExtractedConfigurationFile(const QDir& configurationDirectory)
{
  // any download other than the default from Esri should have it's own DsaAppConfig.json
  // file present. if it is not present, the default will be created when the app is
  // relaunched with this configuration selected.
  const auto dsaAppConfigFilePath = configurationDirectory.absoluteFilePath(DsaUtility::FILE_NAME_APP_CONFIG);
  QFile dsaAppConfigFile{dsaAppConfigFilePath};
  if (!dsaAppConfigFile.exists() || !dsaAppConfigFile.open(QIODevice::ReadOnly))
    return false;

  // parse the file as a json document
  QJsonParseError parseError;
  const QByteArray configBytes{dsaAppConfigFile.readAll()};
  const auto configDoc = QJsonDocument::fromJson(configBytes, &parseError);
  dsaAppConfigFile.close();
  if (configDoc.isNull() || configDoc.isEmpty() || !configDoc.isObject())
    return false;

  // find the previous root data directory value that was configured
  auto configObj = configDoc.object();
  const auto rootDataDirectoryVar = configObj.value(AppConstants::PROPERTYNAME_ROOT_DATA_DIRECTORY);
  if (rootDataDirectoryVar.isUndefined())
    return false;

  // strip off any trailing slashes if they exist
  auto rootDataDirectoryCleaned = rootDataDirectoryVar.toString();
  while (rootDataDirectoryCleaned.back() == '\\' || rootDataDirectoryCleaned.back() == '/')
    rootDataDirectoryCleaned.chop(1);

  // overwrite the app config file with the new json doc
  if (!dsaAppConfigFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return false;

  // re-write the entire document with the old root directory replaced by the extracted location
  QString configString{configBytes};
  const QString rootDataDirectoryNew{configurationDirectory.absolutePath()};
  const QString configStringUpdated{configString.replace(rootDataDirectoryCleaned, rootDataDirectoryNew)};
  dsaAppConfigFile.write(configStringUpdated.toUtf8());
  return true;
}

void ConfigurationController::resetConfigurationDeviceStatus(const QString& configurationName)
{
  if (!m_configurationListModel->setDataByName(configurationName, 0, ConfigurationListModel::PercentDownloaded) ||
      !m_configurationListModel->setDataByName(configurationName, 0, ConfigurationListModel::PercentExtracted))
    emit toolErrorOccurred(QStringLiteral("Error resetting the configurations status on the device"), QStringLiteral("Configuration Error"));
}

void ConfigurationController::download(int index)
{
  // fetch a copy of the selected model item from the listview
  const auto configuration = m_configurationListModel->at(index);
  const auto configurationUrl = configuration.url();
  const auto configurationName = configuration.name();
  if (configurationUrl.isLocalFile())
  {
    m_configurationListModel->setDataByName(configurationName, 100, ConfigurationListModel::PercentDownloaded);
    extractConfigurationDownload(configurationUrl.toLocalFile(), configurationName);
    return;
  }

  // attempt to download the data as a portal item if it matches the item id url pattern
  const auto configurationUrlStr = configuration.urlStr();
  static QRegularExpression regexPortalItem{REGEX_PORTAL_ITEM_URL};
  if (regexPortalItem.match(configurationUrlStr).hasMatch())
  {
    PortalItem checkPortalItem{configurationUrl}; // using a local portal item here to parse the portal and item id out of the url
    auto* portal = new Portal(checkPortalItem.portal()->url(), this);
    auto* portalItem = new PortalItem(portal, checkPortalItem.itemId(), portal);
    connect(portalItem, &PortalItem::doneLoading, this, [this, portal, portalItem, configurationName](const Error& error)
    {
      if (!error.isEmpty())
      {
        emit toolErrorOccurred(QString("Error fetching portal item info: %1").arg(error.message()), QStringLiteral("Download Error"));
        portal->deleteLater();
        return;
      }

      // make sure the device has enough room for the download
      if (!deviceHasRoomForDownload(portalItem->size()))
      {
        emit toolErrorOccurred(QStringLiteral("The total download size exceeds storage available on the device"), QStringLiteral("Download Error"));
        portal->deleteLater();
        return;
      }

      const auto downloadFilePath = generateUniqueDownloadFilePath();
      m_configurationListModel->download(configurationName);
      portalItem->fetchDataAsync(downloadFilePath).then(this, [this, portal, downloadFilePath, configurationName]()
      {
        m_configurationListModel->setDataByName(configurationName, 100, ConfigurationListModel::PercentDownloaded);
        portal->deleteLater();
        extractConfigurationDownload(downloadFilePath, configurationName);
      }).onFailed(this, [this, configurationName, portal, downloadFilePath] (const ErrorException& e)
      {
        portal->deleteLater();
        resetConfigurationDeviceStatus(configurationName);
        removeDownloadedFile(downloadFilePath);
        emit toolErrorOccurred(QString("Downloading the item failed: %1").arg(e.what()), QStringLiteral("Download Error"));
      });
      connect(portalItem, &PortalItem::fetchDataProgressChanged, this, [this, configurationName](const NetworkRequestProgress& progress)
      {
        m_configurationListModel->setDataByName(configurationName, progress.progressPercentage(), ConfigurationListModel::PercentDownloaded);
      });
    });
    portalItem->load();
    return;
  }

  // make a head request to check the availabiity of the network as well as get the final size of the package to be downloaded
  QNetworkRequest zipRequest{configurationUrl};
  auto* headReply = m_networkAccessManager.head(zipRequest);
  headReply->ignoreSslErrors();
  connect(headReply, &QNetworkReply::finished, this, [this, zipRequest, headReply, configurationName, configurationUrl]()
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
      emit toolErrorOccurred(QStringLiteral("Unable to check the download size. Please verify there is enough room on the device for downloading this configuration."), QStringLiteral("Warning"));
    }

    // make sure the device has enough room for the download
    if (!deviceHasRoomForDownload(bytesToDownload))
      return;

    // download the actual file
    auto* contentReply = m_networkAccessManager.get(zipRequest);
    contentReply->ignoreSslErrors();
    m_configurationListModel->download(configurationName);
    const auto downloadFilePath = generateUniqueDownloadFilePath();
    connect(contentReply, &QNetworkReply::downloadProgress, this, [this, contentReply, configurationName](quint64 bytesReceived, quint64 bytesTotal)
    {
      if (isDownloadCancelled(configurationName))
      {
        contentReply->abort();
        return;
      }

      // guard against dividing by zero
      if (bytesTotal == 0)
        return;

      m_configurationListModel->setDataByName(configurationName, bytesReceived * 1.0 / bytesTotal * 1.0 * 100, ConfigurationListModel::PercentDownloaded);
    });
    connect(contentReply, &QNetworkReply::readyRead, this, [this, contentReply, downloadFilePath, configurationName]()
    {
      readyRead(contentReply, downloadFilePath, configurationName);
    });
    connect(contentReply, &QNetworkReply::finished, this, [this, contentReply, downloadFilePath, configurationName]()
    {
      finished(contentReply, downloadFilePath, configurationName);
    });
    connect(contentReply, &QNetworkReply::errorOccurred, this, [this, contentReply](QNetworkReply::NetworkError error)
    {
      contentReply->abort();
      if (error != QNetworkReply::NetworkError::OperationCanceledError)
        emit toolErrorOccurred(QString("Network Error (%1)").arg(contentReply->errorString()), QStringLiteral("Error Downloading"));
    });
  });
}

void ConfigurationController::cancel(int index)
{
  // cancel the downloading item and reset all the
  m_configurationListModel->cancel(index);
}

void ConfigurationController::remove(int index, bool alsoRemoveEntry)
{
  // remove the directory containing the configuration files recursively
  const auto& configuration = m_configurationListModel->at(index);
  QDir configurationDirectory{m_configurationsDirectory.filePath(configuration.name())};
  if (configurationDirectory.exists())
  {
    configurationDirectory.removeRecursively();
    resetConfigurationDeviceStatus(configuration.name());
  }

  // if user also requested to remove the configuration from the list
  // remove it's corresponding model and update the list model
  if (alsoRemoveEntry)
  {
    m_configurationListModel->remove(index);
    storeConfigurations();
  }

  emit configurationsChanged();
}

void ConfigurationController::storeConfigurations()
{
  QJsonArray array;
  for (const auto& cfg : *m_configurationListModel)
  {
    QJsonObject obj;
    obj["selected"] = cfg.selected();
    obj["name"] = cfg.name();
    obj["url"] = cfg.urlStr();
    array.append(obj);
  }
  const QJsonDocument doc{array};
  QFile file{DsaUtility::configurationsFilePath()};
  if (file.open(QIODevice::WriteOnly))
    file.write(doc.toJson());
}

bool ConfigurationController::isDownloadCancelled(const QString& configurationName)
{
  return m_configurationListModel->dataByName(configurationName, ConfigurationListModel::DownloadCancelled).toBool();
}

bool ConfigurationController::deviceHasRoomForDownload(qint64 bytesToDownload)
{
  QStorageInfo storageInfo(m_downloadFolder);
  auto bytesAvailable = storageInfo.bytesAvailable() * 0.95;
  bool enoughSpaceAvailable = bytesToDownload * 2.5 < bytesAvailable;
  if (!enoughSpaceAvailable)
    emit toolErrorOccurred("The expected download/extracted file size exceeds storage available on the device", "Download Error");

  return enoughSpaceAvailable;
}

QString ConfigurationController::generateUniqueDownloadFilePath() const
{
  return m_downloadFolder.absoluteFilePath((QString("dsa_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces))));
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

void ConfigurationController::readyRead(QNetworkReply* networkReply, const QString& downloadFilePath, const QString& configurationName)
{
  // check if the configuration download associated with this request was cancelled by the user
  if (isDownloadCancelled(configurationName))
  {
    networkReply->abort();
    return;
  }

  // make sure the download file can be opened for appending
  QFile file{downloadFilePath};
  if (!file.open(QIODevice::Append))
  {
    networkReply->abort();
    return;
  }

  // append the latest chunk to the download file in progress
  file.write(networkReply->readAll());
}

void ConfigurationController::finished(QNetworkReply* networkReply, const QString& downloadFilePath, const QString& configurationName)
{
  // check if the configuration download associated with this request was cancelled by the user
  if (isDownloadCancelled(configurationName))
  {
    // remove any remaining progress if request was aborted
    networkReply->abort();
    removeDownloadedFile(downloadFilePath);
    return;
  }

  // scope block to cleanup qfile to finish the writing of the stream
  {
    QFile fileFinish{downloadFilePath};
    if (!fileFinish.open(QIODevice::Append))
    {
      emit toolErrorOccurred("Unable to complete download", "Error Downloading");
      return;
    }

    fileFinish.write(networkReply->readAll());
  }

  m_configurationListModel->setDataByName(configurationName, 100, ConfigurationListModel::PercentDownloaded);
  extractConfigurationDownload(downloadFilePath, configurationName);
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

void ConfigurationController::addConfiguration(const QString& url, const QString& name)
{
  // add the new entry
  m_configurationListModel->add(name, url, false, false, 0);
  storeConfigurations();
  emit configurationsChanged();
}

void ConfigurationController::fetchConfigurations()
{
  // check for the default configurations file and create if it doesn't exist
  m_configurationListModel->clear();
  QFile fileConfigurations{DsaUtility::configurationsFilePath()};

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
    auto configurationDirectoryPath = m_configurationsDirectory.filePath(configurationName);
    QDir dirConfiguration{configurationDirectoryPath};

    // skip directories that are not found on disk
    if (!dirConfiguration.exists())
    {
      // add a new empty entry as a place holder for downloading later
      m_configurationListModel->add(configurationName, configurationObject["url"].toString(), false, false, 0);
      continue;
    }

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
