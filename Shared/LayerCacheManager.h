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

class LayerCacheManager : public AbstractTool
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

  void addExcludedPath(const QString& exludedPath);

  void addElevation(const QVariantMap& properties);
  void addLayers(const QVariantMap& properties);

signals:
  void layerJsonChanged();
  void jsonToLayerCompleted(Esri::ArcGISRuntime::Layer* layer);

private slots:
  void onLayerListChanged();

private:
  void connectSignals();

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
  AddLocalDataController* m_localDataController = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
  QHash<int, Esri::ArcGISRuntime::Layer*> m_initialLayerCache;
  QStringList m_excludedPaths;
  QVariantMap m_initialSettings;
  QMetaObject::Connection m_layerAddedConnection;
  QMetaObject::Connection m_dataChangedConnection;
  QMetaObject::Connection m_layerRemovedConnection;
  QMetaObject::Connection m_layoutChangedConnection;
  QMetaObject::Connection m_modelResetConnection;
  QMetaObject::Connection m_layerCreatedConnection;
};

} // Dsa

#endif // LAYERCACHEMANAGER_H
