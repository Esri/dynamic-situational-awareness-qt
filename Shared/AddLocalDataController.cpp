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
#include "ElevationSource.h"
#include "RasterElevationSource.h"
#include "ArcGISTiledElevationSource.h"
#include "LayerListModel.h"
#include "Scene.h"
#include "ShapefileFeatureTable.h"

#include "ToolResourceProvider.h"
#include "ToolManager.h"

#include "DsaUtility.h"
#include "AddLocalDataController.h"
#include "DataItemListModel.h"

using namespace Esri::ArcGISRuntime;

const QString AddLocalDataController::LOCAL_DATAPATHS_PROPERTYNAME = "LocalDataPaths";

const QString AddLocalDataController::s_allData = QStringLiteral("All Data (*.geodatabase *tpk *shp *gpkg *mmpk *kml *slpk *vtpk *.img *.tif *.tiff *.i1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)");
const QString AddLocalDataController::s_rasterData = QStringLiteral("Raster Files (*.img *.tif *.tiff *.I1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)");
const QString AddLocalDataController::s_geodatabaseData = QStringLiteral("Geodatabase (*.geodatabase)");
const QString AddLocalDataController::s_shapefileData = QStringLiteral("Shapefile (*.shp)");
const QString AddLocalDataController::s_kmlData = QStringLiteral("KML (*.kml)");
const QString AddLocalDataController::s_geopackageData = QStringLiteral("GeoPackage (*.gpkg)");
const QString AddLocalDataController::s_sceneLayerData = QStringLiteral("Scene Layer Package (*.slpk)");
const QString AddLocalDataController::s_vectorTilePackageData = QStringLiteral("Vector Tile Package (*.vtpk)");
const QString AddLocalDataController::s_tilePackageData = QStringLiteral("Tile Package (*.tpk)");

AddLocalDataController::AddLocalDataController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_localDataModel(new DataItemListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  // add the base path to the string list
  addPathToDirectoryList(DsaUtility::dataPath());

  // create file filter list
  m_fileFilterList = QStringList{allData(), rasterData(), geodatabaseData(),
      sceneLayerData(), tilePackageData(), shapefileData() /*, kmlData(),
      geopackageData(), vectorTilePackageData()*/}; // uncomment these as new formats are supported
  emit fileFilterListChanged();
  emit localDataModelChanged();
}

QAbstractListModel* AddLocalDataController::localDataModel() const
{
  return m_localDataModel;
}

// The model will contain data items from all of the paths in the list
void AddLocalDataController::addPathToDirectoryList(const QString& path)
{
  if (m_dataPaths.contains(path))
    return;

  m_dataPaths << path;
  emit propertyChanged(LOCAL_DATAPATHS_PROPERTYNAME, m_dataPaths);
}

// clear and re-fetch files in list of data paths
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

// get the file filter string list for filtering data from the QDir entrylist
QStringList AddLocalDataController::determineFileFilters(const QString& fileType)
{
  QStringList fileFilter;
  QStringList rasterExtensions{"*.img", "*.tif", "*.tiff", "*.i1", "*.dt0", "*.dt1", "*.dt2", "*.tc2", "*.geotiff", "*.hr1", "*.jpg", "*.jpeg", "*.jp2", "*.ntf", "*.png", "*.i21"};

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
  else
  {
    fileFilter = rasterExtensions;
    fileFilter << "*.geodatabase" << "*.tpk" << "*.shp" << "*.gpkg" << "*.kml" << "*.slpk" << "*.vtpk";
  }

  return fileFilter;
}

// Q_INVOKABLE function that takes the indices passed in, gets the path and data type
// for the file at the given index, and adds the file as an elevation source
void AddLocalDataController::addItemAsElevationSource(const QList<int>& indices)
{
  QStringList dataPaths;

  for (const int index : indices)
  {
    const DataType dataItemType = m_localDataModel->getDataItemType(index);
    const QString dataItemPath = m_localDataModel->getDataItemPath(index);

    if (dataItemType == DataType::TilePackage)
    {
      TileCache* tileCache = new TileCache(dataItemPath, this);
      TileImageFormat format = tileCache->tileInfo().format();

      // Check if the tiles are LERC encoded
      if (format == TileImageFormat::LERC || format == TileImageFormat::Unknown) // remove the Unknown clause once API bug is fixed
      {
        // create the source from the tiled source
        ArcGISTiledElevationSource* source = new ArcGISTiledElevationSource(tileCache, this);

        connect(source, &ArcGISTiledElevationSource::errorOccurred, this, &AddLocalDataController::errorOccurred);

        source->setParent(this);
        auto scene = Toolkit::ToolResourceProvider::instance()->scene();
        if (scene)
          scene->baseSurface()->elevationSources()->append(source);

        emit elevationSourceSelected(source);
      }
      else
        continue;
    }
    else if (dataItemType == DataType::Raster)
    {
      // add the path to a string list
      dataPaths << dataItemPath;
    }
    else
      continue;
  }

  if (dataPaths.isEmpty())
    return;

  RasterElevationSource* source = new RasterElevationSource(dataPaths, this);

  connect(source, &RasterElevationSource::errorOccurred, this, &AddLocalDataController::errorOccurred);

  source->setParent(this);
  auto scene = Toolkit::ToolResourceProvider::instance()->scene();
  if (scene)
    scene->baseSurface()->elevationSources()->append(source);

  emit elevationSourceSelected(source);
}

// Q_INVOKABLE function that takes the indices passed in, gets the path and data type
// for the file at the given index, and adds the file as a layer
void AddLocalDataController::addItemAsLayer(const QList<int>& indices)
{
  for (const int index : indices)
  {
    DataType dataItemType = m_localDataModel->getDataItemType(index);
    QString dataItemPath = m_localDataModel->getDataItemPath(index);

    switch (dataItemType) {
    case DataType::Geodatabase:
      createFeatureLayerGeodatabase(dataItemPath);
      break;
    case DataType::GeoPackage:
      createLayerGeoPackage(dataItemPath);
      break;
    case DataType::KML:
      createKmlLayer(dataItemPath);
      break;
    case DataType::Raster:
      createRasterLayer(dataItemPath);
      break;
    case DataType::SceneLayerPackage:
      createSceneLayer(dataItemPath);
      break;
    case DataType::Shapefile:
      createFeatureLayerShapefile(dataItemPath);
      break;
    case DataType::TilePackage:
      createTiledLayer(dataItemPath);
      break;
    case DataType::VectorTilePackage:
      createVectorTiledLayer(dataItemPath);
      break;
    default:
      break;
    }
  }
}

// Helper that creates a FeatureLayer for each table in the Geodatabase
void AddLocalDataController::createFeatureLayerGeodatabase(const QString& path)
{
  Geodatabase* gdb = new Geodatabase(path, this);
  connect(gdb, &Geodatabase::doneLoading, this, [this, gdb](Error e)
  {
    if (!e.isEmpty())
      return;

    for (FeatureTable* featureTable : gdb->geodatabaseFeatureTables())
    {
      FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);

      connect(featureLayer, &FeatureLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

      auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
      if (operationalLayers)
        operationalLayers->append(featureLayer);

      emit layerSelected(featureLayer);
    }
  });
  gdb->load();
}

// Helper that creates a Layer for each table and raster in the GeoPackage
void AddLocalDataController::createLayerGeoPackage(const QString& path)
{
  qDebug() << "TODO. Geopackage not yet supported";
  Q_UNUSED(path);
}

// Helper that creates a FeatureLayer from the Shapefile FeatureTable
void AddLocalDataController::createFeatureLayerShapefile(const QString& path)
{
  ShapefileFeatureTable* shpFt = new ShapefileFeatureTable(path, this);
  FeatureLayer* featureLayer = new FeatureLayer(shpFt, this);
  emit layerSelected(featureLayer);
}

// Helper that creates a RasterLayer from a raster file path
void AddLocalDataController::createRasterLayer(const QString& path)
{
  Raster* raster = new Raster(path, this);
  RasterLayer* rasterLayer = new RasterLayer(raster, this);

  connect(rasterLayer, &RasterLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);
  auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
    operationalLayers->append(rasterLayer);

  emit layerSelected(rasterLayer);
}

// Helper that creates a KMLLayer from a file path
void AddLocalDataController::createKmlLayer(const QString& path)
{
  qDebug() << "TODO. KML not yet supported";
  Q_UNUSED(path);
}

// Helper that creates an ArcGISSceneLayer from a slpk path
void AddLocalDataController::createSceneLayer(const QString& path)
{
  ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl(path), this);

  connect(sceneLayer, &ArcGISSceneLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);
  auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
    operationalLayers->append(sceneLayer);

  emit layerSelected(sceneLayer);
}

// Helper that creates an ArcGISTiledLayer from a tpk path
void AddLocalDataController::createTiledLayer(const QString& path)
{
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(QUrl(path), this);

  connect(tiledLayer, &ArcGISTiledLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);
  auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
    operationalLayers->append(tiledLayer);

  emit layerSelected(tiledLayer);
}

// Helper that creates an ArcGISVectorTiledLayer from a vtpk path
void AddLocalDataController::createVectorTiledLayer(const QString& path)
{
  ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl(path), this);

  connect(vectorTiledLayer, &ArcGISVectorTiledLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);
  auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
    operationalLayers->append(vectorTiledLayer);

  emit layerSelected(vectorTiledLayer);
}

QString AddLocalDataController::toolName() const
{
  return QStringLiteral("add local data");
}

void AddLocalDataController::setProperties(const QVariantMap& properties)
{
  const QStringList filePaths = properties[LOCAL_DATAPATHS_PROPERTYNAME].toStringList();
  if (filePaths.empty())
    return;

  for (const QString& filePath : filePaths)
    addPathToDirectoryList(filePath);

  refreshLocalDataModel();
}
