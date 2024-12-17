/*******************************************************************************
 *  Copyright 2012-2024 Esri
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

#include "pch.hpp"

#include "ConfigurationController.h"

#include "Configuration.h"
#include "ConfigurationListModel.h"
#include "DsaUtility.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QUuid>

namespace Dsa {

const QString ConfigurationController::URL_DEFAULT_DOWNLOAD = QString("https://usdanrcswmx.esri.com/arcgis/sharing/rest/content/items/46c2b274325c4418833624d48cb2a44a/data");

ConfigurationController::ConfigurationController(QObject* parent /* = nullptr */):
  AbstractTool(parent),
  m_configurationListModel(new ConfigurationListModel{this})
{                              //name,     url,                                           downloaded, selected, loaded, percentDownloaded
  m_configurationListModel->add("Default", ConfigurationController::URL_DEFAULT_DOWNLOAD, false,      false,    false,  0);

  connect(m_configurationListModel, &ConfigurationListModel::dataChanged, this, [this]() { emit configurationsChanged(); });

  ToolManager::instance().addTool(this);

  // set the downloads folder based on the system type
#if defined Q_OS_IOS || defined Q_OS_ANDROID
  m_downloadFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  m_downloadFolder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
#endif
}

void ConfigurationController::select(int index)
{
  m_configurationListModel->select(index);
  emit configurationsChanged();
}

void ConfigurationController::download(int index)
{
  // prevent calling the download function if something is already in progress
  if (m_downloading)
    return;

  // get the url from the selected item
  auto i = m_configurationListModel->index(index, 0);
  auto url = m_configurationListModel->data(i, ConfigurationListModel::Roles::Url);
  m_activeDownloadIndex = index;

  // generate a unique name for the download
  QDir downloadFolder(m_downloadFolder);
  m_downloadFileName = downloadFolder.absoluteFilePath((QString("dsa_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces))));
  QNetworkRequest networkRequest{QUrl{url.toString()}};

  // reset the properties for download status and use the network manager to download the file
  m_aborted = false;
  m_downloading = true;
  m_networkReply = m_networkAccessManager.get(networkRequest);
  connect(m_networkReply, &QNetworkReply::downloadProgress, this, &ConfigurationController::downloadProgress);
  connect(m_networkReply, &QNetworkReply::readyRead, this, &ConfigurationController::readyRead);
  connect(m_networkReply, &QNetworkReply::finished, this, &ConfigurationController::finished);
}

void ConfigurationController::cancel(int index)
{
  Q_UNUSED(index); // TODO: supporting multiple downloads at a time will require additional management
  if (m_networkReply)
  {
    if (m_networkReply != nullptr)
    {
      if (m_networkReply->isOpen())
      {
        m_configurationListModel->cancel(index);
        m_aborted = true;
        m_downloading = false;
        m_networkReply->abort();
        m_networkReply->deleteLater();
        m_networkReply = nullptr;
      }
    }
  }
}

void ConfigurationController::remove(int index)
{

}

QString ConfigurationController::toolName() const
{
  return QStringLiteral("configurations");
}

void ConfigurationController::setProperties(const QVariantMap& properties)
{
  Q_UNUSED(properties);
}

bool ConfigurationController::downloading()
{
  return m_downloading;
}

QAbstractListModel* ConfigurationController::configurations() const
{
  return m_configurationListModel;
}

void ConfigurationController::downloadProgress(quint64 bytesReceived, quint64 bytesTotal)
{
  setPercentComplete(bytesReceived * 1.0 / bytesTotal * 1.0 * 100);
}

void ConfigurationController::readyRead()
{
  // make sure the download file can be opened for appending
  QFile file(m_downloadFileName);
  if (!file.open(QIODevice::Append))
  {
    // write some abort code
    m_aborted = true;
    return;
  }

  // append the latest chunk to the download file in progress
  file.write(m_networkReply->readAll());
}

void ConfigurationController::finished()
{
  // turn off the downloading state and clean up any aborted download files remaining
  m_downloading = false;
  if (m_aborted)
  {
    QFile fileAbort(m_downloadFileName);
    if (fileAbort.exists())
      fileAbort.remove();
    m_aborted = false;
    setPercentComplete(0);
    return;
  }

  // scope block to cleanup qfile to finish the writing of the stream
  {
    QFile fileFinish{m_downloadFileName};
    if (!fileFinish.open(QIODevice::Append))
      return;

    fileFinish.write(m_networkReply->readAll());
    fileFinish.flush();
  }
  setPercentComplete(100);

  // extract the downloaded file with a new instance of the ziphelper class
  m_zipHelper = new ZipHelper(m_downloadFileName, this);
  connect(m_zipHelper, &ZipHelper::extractCompleted, this, &ConfigurationController::extractCompleted);
  connect(m_zipHelper, &ZipHelper::extractProgress, this, &ConfigurationController::extractProgress);
  connect(m_zipHelper, &ZipHelper::extractError, this, &ConfigurationController::extractError);
  m_zipHelper->extractAll(DsaUtility::dataPath());
}

void ConfigurationController::extractCompleted()
{
  // delete the temporary download from the device
  QFile::remove(m_downloadFileName);
  setPercentComplete(100);
  m_zipHelper->deleteLater();
  emit downloadCompleted();
}

void ConfigurationController::extractProgress(const QString& fileName, const QString& outputFileName, qreal percent)
{
  Q_UNUSED(fileName);
  Q_UNUSED(outputFileName);
  qDebug() << "extracted: " << percent;
}

void ConfigurationController::extractError(const QString& fileName, const QString& outputFileName, ZipHelper::Result result)
{
  Q_UNUSED(result);
  emit toolErrorOccurred(QString("error %1, %2").arg(fileName, outputFileName), "extractError");
}

void ConfigurationController::setPercentComplete(int percentComplete)
{
  auto i = m_configurationListModel->index(m_activeDownloadIndex, 0);
  m_configurationListModel->setData(i, percentComplete, ConfigurationListModel::Roles::PercentDownloaded);
  emit configurationsChanged();
}

}
