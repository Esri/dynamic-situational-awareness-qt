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

#ifndef CONFIGURATIONCONTROLLER_H
#define CONFIGURATIONCONTROLLER_H

// Qt headers
#include <QDir>
#include <QNetworkAccessManager>

// DSA headers
#include "AbstractTool.h"

class QNetworkReply;
Q_MOC_INCLUDE("QAbstractListModel")
class QAbstractListModel;

namespace Esri::ArcGISRuntime {
class PortalItem;
}

namespace Dsa {

class ConfigurationListModel;

class ConfigurationController : public AbstractTool
{
  inline static const QString DEFAULT_DOWNLOAD_URL = QStringLiteral("https://www.arcgis.com/sharing/rest/content/items/02daf003b91348f5a761816e80cc39b5/data");
  inline static const QString DEFAULT_DOWNLOAD_NAME = QStringLiteral("Default");

  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* configurations READ configurations NOTIFY configurationsChanged)
  Q_PROPERTY(bool requiresRestart READ requiresRestart NOTIFY configurationsChanged)
  Q_PROPERTY(bool configurationIsAvailable READ configurationIsAvailable)

public:
  explicit ConfigurationController(QObject* parent = nullptr);
  inline static const QString REGEX_PORTAL_ITEM_URL = QStringLiteral(R"(^https:\/\/.+\/item\.html\?id=[A-Fa-f0-9]{32}?$)");

  Q_INVOKABLE void select(int index);
  Q_INVOKABLE void download(int index);
  Q_INVOKABLE void cancel(int index);
  Q_INVOKABLE void remove(int index, bool alsoRemoveEntry);
  Q_INVOKABLE void downloadDefaultData();
  Q_INVOKABLE void addConfiguration(const QString& url, const QString& name);
  static bool createDefaultConfigurationsFile();

  QString toolName() const override;
  bool requiresRestart();
  bool configurationIsAvailable();

  QAbstractListModel* configurations() const;

signals:
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  void configurationsChanged();

private:
  void fetchConfigurations();
  void storeConfigurations();
  bool isDownloadCancelled(const QString& configurationName);
  bool deviceHasRoomForDownload(qint64 bytesToDownload);
  void readyRead(QNetworkReply* networkReply, const QString& downloadFilePath, const QString& configurationName);
  void finished(QNetworkReply* networkReply, const QString& downloadFilePath, const QString& configurationName);
  QString generateUniqueDownloadFilePath() const;
  void extractConfigurationDownload(const QString& pathToDownload, const QString& configurationName);
  bool updateExtractedConfigurationFile(const QDir& configurationDirectory);
  void resetConfigurationDeviceStatus(const QString& configurationName);
  ConfigurationListModel* m_configurationListModel = nullptr;
  QNetworkAccessManager m_networkAccessManager;
  QDir m_downloadFolder;
  QDir m_configurationsDirectory;
};

}

#endif // CONFIGURATIONCONTROLLER_H
