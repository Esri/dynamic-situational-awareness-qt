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

#ifndef ADDLOCALDATACONTROLLER_H
#define ADDLOCALDATACONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>
#include <QStringList>

namespace Esri {
namespace ArcGISRuntime {
  class Layer;
  class ElevationSource;
}
}

namespace Dsa {

class DataItemListModel;

class AddLocalDataController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
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
  static const QString LOCAL_DATAPATHS_PROPERTYNAME;
  static const QString DEFAULT_ELEVATION_PROPERTYNAME;
};

} // Dsa

#endif // ADDLOCALDATACONTROLLER_H
