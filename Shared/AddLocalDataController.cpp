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

#include <QDir>
#include <QDebug>
#include <QFileInfo>

#include "ArcGISTiledLayer.h"
#include "RasterLayer.h"
#include "Raster.h"
#include "FeatureLayer.h"
#include "GeodatabaseFeatureTable.h"
#include "ArcGISVectorTiledLayer.h"
#include "ArcGISSceneLayer.h"
#include "Geodatabase.h"

#include "DsaUtility.h"
#include "AddLocalDataController.h"
#include "DataItemListModel.h"

using namespace Esri::ArcGISRuntime;

const QString AddLocalDataController::s_allData = QStringLiteral("All Data (*.geodatabase *tpk *shp *gpkg *mmpk *kml *slpk *vtpk *.img *.tif *.tiff *.I1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)");
const QString AddLocalDataController::s_rasterData = QStringLiteral("Raster Files (*.img *.tif *.tiff *.I1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)");
const QString AddLocalDataController::s_geodatabaseData = QStringLiteral("Geodatabase (*.geodatabase)");
const QString AddLocalDataController::s_shapefileData = QStringLiteral("Shapefile (*.shp)");
const QString AddLocalDataController::s_kmlData = QStringLiteral("KML (*.kml)");
const QString AddLocalDataController::s_geopackageData = QStringLiteral("GeoPackage (*.gpkg)");
const QString AddLocalDataController::s_sceneLayerData = QStringLiteral("Scene Layer Package (*.slpk)");
const QString AddLocalDataController::s_vectorTilePackageData = QStringLiteral("Vector Tile Package (*.vtpk)");
const QString AddLocalDataController::s_tilePackageData = QStringLiteral("Tile Package (*.tpk)");

AddLocalDataController::AddLocalDataController(QObject* parent /* = nullptr */):
  QObject(parent),
  m_localDataModel(new DataItemListModel(this))
{
  // placeholder until we have ToolManager
  DsaUtility::tools.append(this);

  // add the base path to the string list
  addPathToDirectoryList(DsaUtility::dataPath());

  // create file filter list
  m_fileFilterList = QStringList{allData(), rasterData(), geodatabaseData(),
                                 /*shapefileData(), kmlData(), geopackageData(),*/
                                 sceneLayerData(), /*vectorTilePackageData(),*/
                                 tilePackageData()};
  emit fileFilterListChanged();
  emit localDataModelChanged();
}

AddLocalDataController::~AddLocalDataController()
{
}

QAbstractListModel* AddLocalDataController::localDataModel() const
{
  return m_localDataModel;
}

void AddLocalDataController::addPathToDirectoryList(const QString& path)
{
  m_dataPaths << path;
}

void AddLocalDataController::refreshLocalDataModel(const QString& fileType)
{
  QStringList fileFilters = determineFileFilters(fileType);
  m_localDataModel->clear();

  for (const QString& path : m_dataPaths)
  {
    QDir localDir(path);

    if (fileFilters.length() > 0)
      localDir.setNameFilters(fileFilters);

    for (const QString& file : localDir.entryList(QDir::Files, QDir::Name))
    {
      QFileInfo fileInfo(localDir, file);
      m_localDataModel->addDataItem(fileInfo.absoluteFilePath());
    }
  }
}

QStringList AddLocalDataController::determineFileFilters(const QString& fileType)
{
  QStringList fileFilter;
  QStringList rasterExtensions{"*.img", "*.tif", "*.tiff", "*.I1", "*.dt0", "*.dt1", "*.dt2", "*.tc2", "*.geotiff", "*.hr1", "*.jpg", "*.jpeg", "*.jp2", "*.ntf", "*.png", "*.i21"};

  if (fileType == geodatabaseData())
    fileFilter << "*.geodatabase";
  else if (fileType == tilePackageData())
    fileFilter << "*.tpk";
  else if (fileType == shapefileData())
    fileFilter << "*.shp";
  else if (fileType == geopackageData())
    fileFilter << "*.gpkg";
  else if (fileType == kmlData())
    fileFilter << "*.kml";
  else if (fileType == sceneLayerData())
    fileFilter << "*.slpk";
  else if (fileType == vectorTilePackageData())
    fileFilter << "*.vtpk";
  else if (fileType == rasterData())
    fileFilter = rasterExtensions;
  else if (fileType == allData())
  {
    fileFilter = rasterExtensions;
    fileFilter << "*.geodatabase" << "*.tpk" << "*.shp" << "*.gpkg" << "*.kml" << "*.slpk" << "*.vtpk";
  }

  return fileFilter;
}

void AddLocalDataController::selectItem(int index)
{
  DataType dataItemType = m_localDataModel->getDataItemType(index);
  QString dataItemPath = m_localDataModel->getDataItemPath(index);

  switch (dataItemType) {
  case DataType::Geodatabase:
    qDebug() << "gdb";
    createFeatureLayerGeodatabase(dataItemPath);
    break;
  case DataType::GeoPackage:
    qDebug() << "gpkg";
    createLayerGeoPackage(dataItemPath);
    break;
  case DataType::KML:
    qDebug() << "kml";
    createKmlLayer(dataItemPath);
    break;
  case DataType::Raster:
    qDebug() << "raster";
    createRasterLayer(dataItemPath);
    break;
  case DataType::SceneLayerPackage:
    qDebug() << "slpk";
    createSceneLayer(dataItemPath);
    break;
  case DataType::Shapefile:
    qDebug() << "shp";
    createFeatureLayerShapefile(dataItemPath);
    break;
  case DataType::TilePackage:
    qDebug() << "tpk";
    createTiledLayer(dataItemPath);
    break;
  case DataType::VectorTilePackage:
    qDebug() << "vtpk";
    createVectorTiledLayer(dataItemPath);
    break;
  default:
    qDebug() << "unknown layer";
    break;
  }
}

void AddLocalDataController::createFeatureLayerGeodatabase(const QString& path)
{
  Geodatabase* gdb = new Geodatabase(path, this);
  connect(gdb, &Geodatabase::doneLoading, this, [this, gdb](Error e)
  {
    if (!e.isEmpty())
      return;
    for (FeatureTable* featureTable : gdb->geodatabaseFeatureTables())
    {
      FeatureLayer* featureLayer = new FeatureLayer(featureTable);
      emit layerSelected(featureLayer);
    }
  });
  gdb->load();
}

void AddLocalDataController::createLayerGeoPackage(const QString& path)
{
  qDebug() << "Geopackage not yet supported";
  Q_UNUSED(path);
}

void AddLocalDataController::createFeatureLayerShapefile(const QString& path)
{
  qDebug() << "Shapefile not yet supported";
  Q_UNUSED(path);
}

void AddLocalDataController::createRasterLayer(const QString& path)
{
  Raster* raster = new Raster(path, this);
  RasterLayer* rasterLayer = new RasterLayer(raster);
  emit layerSelected(rasterLayer);
}

void AddLocalDataController::createKmlLayer(const QString& path)
{
  qDebug() << "KML not yet supported";
  Q_UNUSED(path);
}

void AddLocalDataController::createSceneLayer(const QString& path)
{
  ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl(path));
  emit layerSelected(sceneLayer);
}

void AddLocalDataController::createTiledLayer(const QString& path)
{
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(QUrl(path));
  emit layerSelected(tiledLayer);
}

void AddLocalDataController::createVectorTiledLayer(const QString& path)
{
  ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl(path));
  emit layerSelected(vectorTiledLayer);
}

QString AddLocalDataController::toolName() const
{
  return QStringLiteral("add local data");
}
