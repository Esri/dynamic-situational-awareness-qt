/*******************************************************************************
 *  Copyright 2012-2018 Esri
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

#ifndef DSACONTROLLER_H
#define DSACONTROLLER_H

// Qt headers
#include <QJsonArray>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QVariantMap>

namespace Esri {
namespace ArcGISRuntime {
  class Error;
  class Scene;
  class GeoView;
  class Layer;
}
}

namespace Dsa {

class LayerCacheManager;

class DsaController : public QObject
{
  Q_OBJECT

public:
  DsaController(QObject* parent = nullptr);
  ~DsaController();

  Esri::ArcGISRuntime::Scene* scene() const;

  void init(Esri::ArcGISRuntime::GeoView* geoView);

public slots:
  void onError(const Esri::ArcGISRuntime::Error& error);
  void onToolError(const QString& errorMessage, const QString& additionalMessage);

private slots:
  void onPropertyChanged(const QString& propertyName, const QVariant& propertyValue);

signals:
  void errorOccurred(const QString& message, const QString& additionalMessage);

private:
  void setDefaultViewpoint();
  void setupConfig();
  void createDefaultSettings();
  void saveSettings();
  void writeDefaultLocalDataPaths();
  void writeDefaultConditions();
  void writeDefaultMessageFeeds();
  bool isConflictingTool(const QString& toolName) const;

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  LayerCacheManager* m_cacheManager = nullptr;

  QString m_dataPath;
  QVariantMap m_dsaSettings;
  QString m_configFilePath;
  QSettings::Format m_jsonFormat;
  QStringList m_conflictingToolNames;
};

} // Dsa

#endif // DSACONTROLLER_H
