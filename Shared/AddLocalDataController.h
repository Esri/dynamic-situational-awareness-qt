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

#ifndef ADDLOCALDATACONTROLLER_H
#define ADDLOCALDATACONTROLLER_H

// dsa headers
#include "AbstractTool.h"

// Qt headers
#include <QStringList>

class QAbstractListModel;

namespace Esri::ArcGISRuntime {
  class Layer;
  class ElevationSource;
}

namespace Dsa {

class DataItemListModel;

class AddLocalDataController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* localDataModel READ localDataModel NOTIFY localDataModelChanged)
  Q_PROPERTY(QStringList fileFilterList READ fileFilterList NOTIFY fileFilterListChanged)

public:
  explicit AddLocalDataController(QObject* parent = nullptr);
  ~AddLocalDataController() = default;

  Q_INVOKABLE void addPathToDirectoryList(const QString& path);
  Q_INVOKABLE void refreshLocalDataModel(const QString& fileType = "All");
  Q_INVOKABLE void addItemAsLayer(const QList<int>& index);
  Q_INVOKABLE void addLayerFromPath(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  Q_INVOKABLE void addItemAsElevationSource(const QList<int>& indices);
  QAbstractListModel* localDataModel() const;

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  // helpers for creating the layers for a given string
  void createFeatureLayerGeodatabase(const QString& path);
  void createFeatureLayerGeodatabaseWithId(const QString &path, int layerIndex = -1, int serviceLayerId = 0, bool visible = true, bool autoAdd = true);
  void createLayerGeoPackage(const QString& path);
  void createFeatureLayerGeoPackage(const QString& path, int layerIndex = -1, int id = 0, bool visible = true, bool autoAdd = true);
  void createRasterLayerGeoPackage(const QString& path, int layerIndex = -1, int id = 0, bool visible = true, bool autoAdd = true);
  void createFeatureLayerShapefile(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  void createRasterLayer(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  void createSceneLayer(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  void createTiledLayer(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  void createKmlLayer(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  void createVectorTiledLayer(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  void createElevationSourceFromTpk(const QString& path);
  void createElevationSourceFromRasters(const QStringList& paths);
  Q_INVOKABLE void createMarkupLayer(const QString& path, int layerIndex = -1, bool visible = true, bool autoAdd = true);
  QStringList dataPaths() const { return m_dataPaths; }

signals:
  void localDataModelChanged();
  void layerSelected(Esri::ArcGISRuntime::Layer* layer);
  void elevationSourceSelected(Esri::ArcGISRuntime::ElevationSource* source);
  void fileFilterListChanged();
  void layerCreated(int i, Esri::ArcGISRuntime::Layer* layer);
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

private:
  QStringList determineFileFilters(const QString& fileType);
  QStringList fileFilterList() const { return m_fileFilterList; }
  static const QString allData() { return s_allData; }
  static const QString rasterData() { return s_rasterData; }
  static const QString geodatabaseData() { return s_geodatabaseData; }
  static const QString shapefileData() { return s_shapefileData; }
  static const QString geopackageData() { return s_geopackageData; }
  static const QString sceneLayerData() { return s_sceneLayerData; }
  static const QString vectorTilePackageData() { return s_vectorTilePackageData; }
  static const QString tilePackageData() { return s_tilePackageData; }
  static const QString markupData() { return s_markupData; }
  static const QString kmlData() { return s_kmlData; }

private:
  DataItemListModel* m_localDataModel;
  QStringList m_dataPaths;
  QStringList m_fileFilterList;
  static const QString s_allData;
  static const QString s_rasterData;
  static const QString s_geodatabaseData;
  static const QString s_shapefileData;
  static const QString s_geopackageData;
  static const QString s_sceneLayerData;
  static const QString s_vectorTilePackageData;
  static const QString s_tilePackageData;
  static const QString s_markupData;
  static const QString s_kmlData;
  static const QString LOCAL_DATAPATHS_PROPERTYNAME;
  static const QString DEFAULT_ELEVATION_PROPERTYNAME;
};

} // Dsa

#endif // ADDLOCALDATACONTROLLER_H
