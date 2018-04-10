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

#ifndef LAYERCACHEMANAGER_H
#define LAYERCACHEMANAGER_H

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QJsonArray>

namespace Esri {
namespace ArcGISRuntime {
class Layer;
class Scene;
}
}

namespace Dsa {

class TableOfContentsController;
class AddLocalDataController;

class LayerCacheManager : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:
  explicit LayerCacheManager(QObject* parent = nullptr);
  ~LayerCacheManager();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  void layerToJson(Esri::ArcGISRuntime::Layer* layer);
  void jsonToLayer(const QJsonObject& jsonObject, const int layerIndex = -1);
  QJsonArray layerJson() const;

signals:
  void layerJsonChanged();
  void jsonToLayerCompleted(Esri::ArcGISRuntime::Layer* layer);

private slots:
  void onLayerListChanged();

private:
  static const QString LAYERS_PROPERTYNAME;
  static const QString ELEVATION_PROPERTYNAME;
  static const QString layerPathKey;
  static const QString layerVisibleKey;
  static const QString layerTypeKey;
  static const QString layerIdKey;
  static const QString layerTypeFeatureLayerGeodatabase;
  static const QString layerTypeFeatureLayerGeoPackage;
  static const QString layerTypeRasterLayerGeoPackage;
  QJsonArray m_layers;
  QJsonArray m_inputLayerJsonArray;
  bool m_initialLoadCompleted = false;
  AddLocalDataController* m_localDataController;
  Esri::ArcGISRuntime::Scene* m_scene;
  QHash<int, Esri::ArcGISRuntime::Layer*> m_initialLayerCache;
};

} // Dsa

#endif // LAYERCACHEMANAGER_H
