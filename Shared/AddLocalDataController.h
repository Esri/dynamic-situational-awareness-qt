// Copyright 2016 ESRI
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

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>

namespace Esri
{
namespace ArcGISRuntime
{
  class Layer;
}
}

class DataItemListModel;

class AddLocalDataController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* localDataModel READ localDataModel NOTIFY localDataModelChanged)
  Q_PROPERTY(QStringList fileFilterList READ fileFilterList NOTIFY fileFilterListChanged)

  enum class DataType
  {
    Raster,
    Geodatabase,
    TilePackage,
    VectorTilePackage,
    SceneLayerPackage,
    Shapefile,
    GeoPackage,
    KML,
    All,
    Unknown
  };

public:
  AddLocalDataController(QObject* parent = nullptr);
  ~AddLocalDataController();

  Q_INVOKABLE void addPathToDirectoryList(const QString& path);
  Q_INVOKABLE void refreshLocalDataModel(const QString& fileType = "All");
  Q_INVOKABLE void selectItem(int index);
  QAbstractListModel* localDataModel() const;
  QString toolName() const;

  // helpers for creating the layers for a given string
  void createFeatureLayerGeodatabase(const QString& path);
  void createLayerGeoPackage(const QString& path);
  void createFeatureLayerShapefile(const QString& path);
  void createRasterLayer(const QString& path);
  void createKmlLayer(const QString& path);
  void createSceneLayer(const QString& path);
  void createTiledLayer(const QString& path);
  void createVectorTiledLayer(const QString& path);

signals:
  void localDataModelChanged();
  void layerSelected(Esri::ArcGISRuntime::Layer* layer);
  void fileFilterListChanged();

private:
  DataItemListModel* m_localDataModel;
  QStringList m_dataPaths;
  QStringList m_fileFilterList;
  QString m_allData = "All Data (*.geodatabase *tpk *shp *gpkg *mmpk *kml *slpk *vtpk *.img *.tif *.tiff *.I1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)";
  QString m_rasterData = "Raster Files (*.img *.tif *.tiff *.I1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)";
  QString m_geodatabaseData = "Geodatabase (*.geodatabase)";
  QString m_shapefileData = "Shapefile (*.shp)";
  QString m_kmlData = "KML (*.kml)";
  QString m_geopackageData = "GeoPackage (*.gpkg)";
  QString m_sceneLayerData = "Scene Layer Package (*.slpk)";
  QString m_vectorTilePackageData = "Vector Tile Package (*.vtpk)";
  QString m_tilePackageData = "Tile Package (*.tpk)";

  QStringList determineFileFilters(const QString& fileType);
  QStringList fileFilterList() const { return m_fileFilterList; }
};

#endif // ADDLOCALDATACONTROLLER_H
