// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

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
  void setupConfig();
  void createDefaultSettings();
  void saveSettings();
  void writeDefaultInitialLocation();
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
