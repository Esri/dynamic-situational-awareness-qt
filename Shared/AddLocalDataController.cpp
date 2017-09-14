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
#include "DataItem.h"
#include "DataItemListModel.h"

using namespace Esri::ArcGISRuntime;

AddLocalDataController::AddLocalDataController(QObject* parent /* = nullptr */):
  QObject(parent),
  m_localDataModel(new DataItemListModel(this))
{
  // placeholder until we have ToolManager
  DsaUtility::tools.append(this);

  // add the base path to the string list
  addPathToDirectoryList(DsaUtility::dataPath());

  // refresh local data model
  refreshLocalDataModel();
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
  foreach (const QString& path, m_dataPaths)
  {
    // NOTE - NEED TO DELETE PREVIOUS DATA ITEMS FROM LIST MODEL
    QDir localDir(path);
    if (fileFilters.length() > 0)
    {
      localDir.setNameFilters(fileFilters);
    }
    foreach (const QString& file, localDir.entryList(QDir::Files, QDir::Name))
    {
      QFileInfo fileInfo(localDir, file);
      DataItem* dataItem = new DataItem(fileInfo.absoluteFilePath());
      m_localDataModel->addDataItem(dataItem);
      emit localDataModelChanged();
    }
  }
}

QStringList AddLocalDataController::determineFileFilters(const QString& fileType)
{
  QStringList fileFilter;
  QStringList rasterExtensions{"*.img", "*.tif", "*.tiff", "*.I1", "*.dt0", "*.dt1", "*.dt2", "*.tc2", "*.geotiff", "*.hr1", "*.jpg", "*.jpeg", "*.jp2", "*.ntf", "*.png", "*.i21"};

  if (fileType == "*.geodatabase")
    fileFilter << "*.geodatabase";
  else if (fileType == "*.tpk")
    fileFilter << "*.tpk";
  else if (fileType == "*.shp")
    fileFilter << "*.shp";
  else if (fileType == "*.mmpk")
    fileFilter << "*.mmpk";
  else if (fileType == "*.gpkg")
    fileFilter << "*.gpkg";
  else if (fileType == "*.kml")
    fileFilter << "*.kml";
  else if (fileType == "*.slpk")
    fileFilter << "*.slpk";
  else if (fileType == "*.vtpk")
    fileFilter << "*.vtpk";
  else if (fileType == "Raster")
    fileFilter = rasterExtensions;
  else if (fileType == "All")
  {
    fileFilter = rasterExtensions;
    fileFilter << "*.geodatabase" << "*.tpk" << "*.shp" << "*.mmpk" << "*.gpkg" << "*.kml" << "*.slpk" << "*.vtpk";
  }

  return fileFilter;
}

void AddLocalDataController::selectItem(int index)
{
  DataItem* dataItem = m_localDataModel->get(index);

  switch (dataItem->dataType()) {
  case DataItem::DataType::Geodatabase:
    createFeatureLayerGeodatabase(dataItem->fullPath());
    break;
  case DataItem::DataType::GeoPackage:
    createLayerGeoPackage(dataItem->fullPath());
    break;
  case DataItem::DataType::KML:
    createKmlLayer(dataItem->fullPath());
    break;
  case DataItem::DataType::MobileMapPackage:
    qDebug() << "not supported yet";
    break;
  case DataItem::DataType::Raster:
    createRasterLayer(dataItem->fullPath());
    break;
  case DataItem::DataType::SceneLayerPackage:
    createSceneLayer(dataItem->fullPath());
    break;
  case DataItem::DataType::Shapefile:
    createFeatureLayerShapefile(dataItem->fullPath());
    break;
  case DataItem::DataType::TilePackage:
    createTiledLayer(dataItem->fullPath());
    break;
  case DataItem::DataType::VectorTilePackage:
    createVectorTiledLayer(dataItem->fullPath());
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
    foreach (FeatureTable* featureTable, gdb->geodatabaseFeatureTables())
    {
      FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);
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
  RasterLayer* rasterLayer = new RasterLayer(raster, this);
  emit layerSelected(rasterLayer);
}

void AddLocalDataController::createKmlLayer(const QString& path)
{
  qDebug() << "KML not yet supported";
  Q_UNUSED(path);
}

void AddLocalDataController::createSceneLayer(const QString& path)
{
  ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl(path), this);
  emit layerSelected(sceneLayer);
}

void AddLocalDataController::createTiledLayer(const QString& path)
{
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(QUrl(path), this);
  emit layerSelected(tiledLayer);
}

void AddLocalDataController::createVectorTiledLayer(const QString& path)
{
  ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl(path), this);
  emit layerSelected(vectorTiledLayer);
}

QString AddLocalDataController::toolName() const
{
  return QStringLiteral("add local data");
}
