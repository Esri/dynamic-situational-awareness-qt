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

namespace Esri {
namespace ArcGISRuntime {
class Layer;
class Scene;
}
}

class TableOfContentsController;
class AddLocalDataController;

#include "AbstractTool.h"
#include <QJsonArray>

class LayerCacheManager : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:
  explicit LayerCacheManager(QObject* parent = nullptr);
  ~LayerCacheManager();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

private slots:
  void onLayerListChanged();

private:
  static const QString LAYERS_PROPERTYNAME;
  static const QString layerPathKey;
  static const QString layerVisibleKey;
  static const QString layerTypeKey;
  static const QString layerIdKey;
  static const QString layerTypeFeatureLayerGeodatabase;
  static const QString layerTypeFeatureLayerGeoPackage;
  static const QString layerTypeRasterLayerGeoPackage;
  QJsonArray m_layers;
  bool m_initialLoadCompleted = false;
  AddLocalDataController* m_localDataController;
  Esri::ArcGISRuntime::Scene* m_scene;
  void layerToJson(Esri::ArcGISRuntime::Layer* layer);
};

#endif // LAYERCACHEMANAGER_H
