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

#include <QDir>
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
#include "SimpleRenderer.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleMarkerSceneSymbol.h"
#include "PictureMarkerSymbol.h"
#include "CompositeSymbol.h"
#include "GeoPackage.h"
#include "GeoPackageRaster.h"
#include "GeoPackageFeatureTable.h"

#include "ToolResourceProvider.h"
#include "ToolManager.h"

#include "DsaUtility.h"
#include "AddLocalDataController.h"
#include "DataItemListModel.h"

using namespace Esri::ArcGISRuntime;

const QString AddLocalDataController::LOCAL_DATAPATHS_PROPERTYNAME = "LocalDataPaths";
const QString AddLocalDataController::DEFAULT_ELEVATION_PROPERTYNAME = "DefaultElevationSource";

const QString AddLocalDataController::s_allData = QStringLiteral("All Data (*.geodatabase *.tpk *.shp *.gpkg *.mmpk *.slpk *.vtpk *.img *.tif *.tiff *.i1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)");
const QString AddLocalDataController::s_rasterData = QStringLiteral("Raster Files (*.img *.tif *.tiff *.I1, *.dt0 *.dt1 *.dt2 *.tc2 *.geotiff *.hr1 *.jpg *.jpeg *.jp2 *.ntf *.png *.i21 *.ovr)");
const QString AddLocalDataController::s_geodatabaseData = QStringLiteral("Geodatabase (*.geodatabase)");
const QString AddLocalDataController::s_shapefileData = QStringLiteral("Shapefile (*.shp)");
const QString AddLocalDataController::s_geopackageData = QStringLiteral("GeoPackage (*.gpkg)");
const QString AddLocalDataController::s_sceneLayerData = QStringLiteral("Scene Layer Package (*.slpk)");
const QString AddLocalDataController::s_vectorTilePackageData = QStringLiteral("Vector Tile Package (*.vtpk)");
const QString AddLocalDataController::s_tilePackageData = QStringLiteral("Tile Package (*.tpk)");

/*
 \brief Constructor that takes an optional \a parent.
 */
AddLocalDataController::AddLocalDataController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_localDataModel(new DataItemListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  // add the base path to the string list
  addPathToDirectoryList(DsaUtility::dataPath());

  // create file filter list
  m_fileFilterList = QStringList{allData(), rasterData(), geodatabaseData(),
      sceneLayerData(), tilePackageData(), shapefileData(), geopackageData()
        /*, vectorTilePackageData()*/}; // VTPK is not supported in 3D
  emit fileFilterListChanged();
  emit localDataModelChanged();
}

/*
 \brief Returns the local data model associated with the controller.
 */
QAbstractListModel* AddLocalDataController::localDataModel() const
{
  return m_localDataModel;
}

/*
 \brief Adds the provided \a path to the directory list.

 The directory list contains all of the directories that will be searched for local data.
 */
void AddLocalDataController::addPathToDirectoryList(const QString& path)
{
  if (m_dataPaths.contains(path))
    return;

  if (!QFileInfo::exists(path))
  {
    emit toolErrorOccurred(QStringLiteral("Failed to add local data path"), QString("Could not find %1").arg(path));
    return;
  }

  m_dataPaths << path;
  emit propertyChanged(LOCAL_DATAPATHS_PROPERTYNAME, m_dataPaths);
}

/*
 \brief Refreshes the local data model with a given \a fileType.
 */
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

/*
 \brief Returns the file filter string list for filtering data from the QDir entrylist
 */
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
  else if (fileType == sceneLayerData())
    fileFilter << "*.slpk";
  else if (fileType == vectorTilePackageData())
    fileFilter << "*.vtpk";
  else if (fileType == rasterData())
    fileFilter = rasterExtensions;
  else
  {
    fileFilter = rasterExtensions;
    fileFilter << "*.geodatabase" << "*.tpk" << "*.shp" << "*.gpkg" << "*.slpk"/* << "*.vtpk"*/; // VTPK is not supported in 3D
  }

  return fileFilter;
}

/*
 \brief Adds the provided \a indices from the list model as elevation sources to the Scene.
 */
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

      connect(tileCache, &TileCache::doneLoading, this, [this, tileCache](Error error)
      {
        if (!error.isEmpty())
          return;

        // Check if the tiles are LERC encoded
        if (tileCache->tileInfo().format() == TileImageFormat::LERC)
        {
          // create the source from the tiled source
          ArcGISTiledElevationSource* source = new ArcGISTiledElevationSource(tileCache, this);

          connect(source, &ArcGISTiledElevationSource::errorOccurred, this, &AddLocalDataController::errorOccurred);

          source->setParent(this);
          auto scene = Toolkit::ToolResourceProvider::instance()->scene();
          if (scene)
            scene->baseSurface()->elevationSources()->append(source);

          emit elevationSourceSelected(source);
          emit propertyChanged(DEFAULT_ELEVATION_PROPERTYNAME, tileCache->path());
        }
      });

      tileCache->load();
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
  emit propertyChanged(DEFAULT_ELEVATION_PROPERTYNAME, dataPaths);
}

/*
 \brief Adds the provided \a indices from the list model as layers.
 */
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

/*
 \brief Determines data type from the given \a path, and calls the appropriate helper.

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::addLayerFromPath(const QString& path, int layerIndex, bool visible, bool autoAdd)
{
  QFileInfo fileInfo(path);
  if (!fileInfo.exists())
  {
    const QUrl testUrl(path);
    if (testUrl.isLocalFile())
      fileInfo = QFileInfo(testUrl.toLocalFile());

    if (!fileInfo.exists())
    {
      emit toolErrorOccurred(QString("Failed to add %1").arg(fileInfo.fileName()), QString("File not found %1").arg(path));
      return;
    }
  }

  QStringList rasterExtensions{"img", "tif", "tiff", "i1", "dt0", "dt1", "dt2", "tc2", "geotiff", "hr1", "jpg", "jpeg", "jp2", "ntf", "png", "i21"};

  // determine the layer type
  QString fileExtension = fileInfo.completeSuffix();

  if (fileExtension == "geodatabase")
    createFeatureLayerGeodatabaseWithId(path, layerIndex, visible, autoAdd);
  else if (fileExtension.compare("tpk", Qt::CaseInsensitive) == 0)
    createTiledLayer(path, layerIndex, visible, autoAdd);
  else if (fileExtension.compare("shp", Qt::CaseInsensitive) == 0)
    createFeatureLayerShapefile(path, layerIndex, visible, autoAdd);
  else if (fileExtension.compare("gpkg", Qt::CaseInsensitive) == 0)
    createLayerGeoPackage(path);
  else if (fileExtension.compare("slpk", Qt::CaseInsensitive) == 0)
    createSceneLayer(path, layerIndex, visible, autoAdd);
  else if (fileExtension.compare("vtpk", Qt::CaseInsensitive) == 0)
    createVectorTiledLayer(path, layerIndex, visible, autoAdd);
  else if (rasterExtensions.contains(fileExtension.toLower()))
    createRasterLayer(path, layerIndex, visible, autoAdd);
}

/*
 \brief Creates a Geodatabase from the given \a path, and creates a FeatureLayer for each table in the Geodatabase.
 */
void AddLocalDataController::createFeatureLayerGeodatabase(const QString& path)
{
  Geodatabase* gdb = new Geodatabase(path, this);
  connect(gdb, &Geodatabase::doneLoading, this, [this, gdb](Error e)
  {
    if (!e.isEmpty())
    {
      emit errorOccurred(e);
      return;
    }

    auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();

    for (FeatureTable* featureTable : gdb->geodatabaseFeatureTables())
    {
      FeatureLayer* featureLayer = new FeatureLayer(featureTable, this);

      connect(featureLayer, &FeatureLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

      connect(featureLayer, &FeatureLayer::doneLoading, this, [this, featureLayer](Error loadError)
      {
        if (!loadError.isEmpty())
          return;

        if (featureLayer->featureTable()->hasZ() && featureLayer->featureTable()->geometryType() == GeometryType::Point)
          featureLayer->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
      });

      if (operationalLayers)
        operationalLayers->append(featureLayer);

      emit layerSelected(featureLayer);
    }
  });
  gdb->load();
}

/*
 \brief Creates a FeatureLayer with the feature table at the given id of a Geodatabase

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a serviceLayerId - The service layer ID of the GeodatabaseFeatureTable.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::createFeatureLayerGeodatabaseWithId(const QString& path, int layerIndex, int serviceLayerId, bool visible, bool autoAdd)
{
  Geodatabase* gdb = new Geodatabase(path, this);
  connect(gdb, &Geodatabase::doneLoading, this, [this, gdb, serviceLayerId, visible, autoAdd, layerIndex](Error e)
  {
    if (!e.isEmpty())
    {
      emit errorOccurred(e);
      return;
    }

    FeatureLayer* featureLayer = new FeatureLayer(gdb->geodatabaseFeatureTable(serviceLayerId), this);
    featureLayer->setVisible(visible);
    connect(featureLayer, &FeatureLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

    connect(featureLayer, &FeatureLayer::doneLoading, this, [this, featureLayer](Error loadError)
    {
      if (!loadError.isEmpty())
        return;

      if (featureLayer->featureTable()->hasZ() && featureLayer->featureTable()->geometryType() == GeometryType::Point)
        featureLayer->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
    });

    if (autoAdd)
    {
      auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
      if (operationalLayers)
        operationalLayers->append(featureLayer);

      emit layerSelected(featureLayer);
      Q_UNUSED(layerIndex)
    }
    else
    {
      emit layerCreated(layerIndex, featureLayer);
    }
  });
  gdb->load();
}

/*
 \brief Creates a FeatureLayer with the feature table at the given index of a GeoPackage

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a id - The index of the feature table in the GeoPackage feature table list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::createFeatureLayerGeoPackage(const QString& path, int layerIndex, int id, bool visible, bool autoAdd)
{
  GeoPackage* geoPackage = new GeoPackage(path, this);

  connect(geoPackage, &GeoPackage::doneLoading, this, [this, geoPackage, id, visible, autoAdd, layerIndex](Error e)
  {
    if (!e.isEmpty())
    {
      emit errorOccurred(e);
      return;
    }

    FeatureLayer* featureLayer = new FeatureLayer(geoPackage->geoPackageFeatureTables().at(id), this);
    featureLayer->setVisible(visible);
    connect(featureLayer, &FeatureLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

    connect(featureLayer, &FeatureLayer::doneLoading, this, [this, featureLayer](Error loadError)
    {
      if (!loadError.isEmpty())
        return;

      if (featureLayer->featureTable()->hasZ() && featureLayer->featureTable()->geometryType() == GeometryType::Point)
        featureLayer->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
    });

    if (autoAdd)
    {
      auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
      operationalLayers->append(featureLayer);
      emit layerSelected(featureLayer);
      Q_UNUSED(layerIndex)
    }
    else
    {
      emit layerCreated(layerIndex, featureLayer);
    }
  });

  geoPackage->load();
}

/*
 \brief Creates a RasterLayer with the raster at the given index of a GeoPackage

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a id - The index of the raster in the GeoPackage raster list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::createRasterLayerGeoPackage(const QString& path, int layerIndex, int id, bool visible, bool autoAdd)
{
  GeoPackage* geoPackage = new GeoPackage(path, this);

  connect(geoPackage, &GeoPackage::doneLoading, this, [this, geoPackage, id, visible, autoAdd, layerIndex](Error e)
  {
    if (!e.isEmpty())
    {
      emit errorOccurred(e);
      return;
    }

    RasterLayer* rasterLayer = new RasterLayer(geoPackage->geoPackageRasters().at(id), this);
    connect(rasterLayer, &RasterLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);
    rasterLayer->setVisible(visible);

    if (autoAdd)
    {
      auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
      if (!operationalLayers)
        return;

      operationalLayers->append(rasterLayer);
      emit layerSelected(rasterLayer);
      Q_UNUSED(layerIndex)
    }
    else
    {
      emit layerCreated(layerIndex, rasterLayer);
    }
  });

  geoPackage->load();
}

/*
 \brief Creates a GeoPackage from the given \a path, and adds a Layer for each table and raster in the GeoPackage
 */
void AddLocalDataController::createLayerGeoPackage(const QString& path)
{
  GeoPackage* geoPackage = new GeoPackage(path, this);

  connect(geoPackage, &GeoPackage::doneLoading, this, [this, geoPackage](Error e)
  {
    if (!e.isEmpty())
    {
      emit errorOccurred(e);
      return;
    }

    auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();

    for (const auto& table : geoPackage->geoPackageFeatureTables())
    {
      FeatureLayer* featureLayer = new FeatureLayer(table, this);
      connect(featureLayer, &FeatureLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

      connect(featureLayer, &FeatureLayer::doneLoading, this, [this, featureLayer](Error loadError)
      {
        if (!loadError.isEmpty())
          return;

        if (featureLayer->featureTable()->hasZ() && featureLayer->featureTable()->geometryType() == GeometryType::Point)
          featureLayer->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
      });

      if (operationalLayers)
        operationalLayers->append(featureLayer);

      emit layerSelected(featureLayer);
    }

    for (const auto& raster : geoPackage->geoPackageRasters())
    {
      RasterLayer* rasterLayer = new RasterLayer(raster, this);
      connect(rasterLayer, &RasterLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);
      if (operationalLayers)
        operationalLayers->append(rasterLayer);

      emit layerSelected(rasterLayer);
    }
  });

  geoPackage->load();
}

/*
 \brief Creates a FeatureLayer with the provided shapefile path.

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::createFeatureLayerShapefile(const QString& path, int layerIndex, bool visible, bool autoAdd)
{
  ShapefileFeatureTable* shpFt = new ShapefileFeatureTable(path, this);
  FeatureLayer* featureLayer = new FeatureLayer(shpFt, this);
  featureLayer->setVisible(visible);
  connect(featureLayer, &FeatureLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

  connect(featureLayer, &FeatureLayer::doneLoading, this, [this, featureLayer](Error loadError)
  {
    if (!loadError.isEmpty())
      return;

    if (featureLayer->featureTable()->hasZ() && featureLayer->featureTable()->geometryType() == GeometryType::Point)
    {
      // Option 1:
//      SimpleMarkerSceneSymbol* symbol = new SimpleMarkerSceneSymbol(SimpleMarkerSceneSymbolStyle::Sphere, QColor("yellow"), 10.0, 7.5, 7.5, SceneSymbolAnchorPosition::Bottom, this);

//       Option 2:
//      SimpleMarkerSymbol* symbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("yellow"), 10, this);

      // Option 3:
      QList<Symbol*> symbols{
        new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("darkgray"), 14, this),
        new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("white"), 12, this),
        new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("yellow"), 10, this)};
      CompositeSymbol* symbol = new CompositeSymbol(symbols, this);

      // Option 4:
//    PictureMarkerSymbol* symbol = new PictureMarkerSymbol(QUrl::fromLocalFile("C:\\Users\\luke8660\\ArcGIS\\Runtime\\UnitTests\\images\\RedShinyPin.png"), this);

      Renderer* renderer = new SimpleRenderer(symbol, this);
      featureLayer->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
      featureLayer->setRenderer(renderer);
    }
  });

  if (autoAdd)
  {
    auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
    if (operationalLayers)
      operationalLayers->append(featureLayer);

    emit layerSelected(featureLayer);
    Q_UNUSED(layerIndex)
  }
  else
  {
    emit layerCreated(layerIndex, featureLayer);
  }
}

/*
 \brief Creates a Rasterlayer with the provided raster path.

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::createRasterLayer(const QString& path, int layerIndex, bool visible, bool autoAdd)
{
  Raster* raster = new Raster(path, this);
  RasterLayer* rasterLayer = new RasterLayer(raster, this);
  rasterLayer->setVisible(visible);
  connect(rasterLayer, &RasterLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

  if (autoAdd)
  {
    auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
    if (operationalLayers)
      operationalLayers->append(rasterLayer);

    emit layerSelected(rasterLayer);
    Q_UNUSED(layerIndex)
  }
  else
  {
    emit layerCreated(layerIndex, rasterLayer);
  }
}

/*
 \brief Creates an ArcGISSceneLayer with the provided scene layer package path.

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::createSceneLayer(const QString& path, int layerIndex, bool visible, bool autoAdd)
{
  ArcGISSceneLayer* sceneLayer = new ArcGISSceneLayer(QUrl(path), this);
  sceneLayer->setVisible(visible);
  connect(sceneLayer, &ArcGISSceneLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

  if (autoAdd)
  {
    auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
    if (operationalLayers)
      operationalLayers->append(sceneLayer);

    emit layerSelected(sceneLayer);
    Q_UNUSED(layerIndex)
  }
  else
  {
    emit layerCreated(layerIndex, sceneLayer);
  }
}

/*
 \brief Creates an ArcGISTiledLayer with the provided TPK path.

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/
void AddLocalDataController::createTiledLayer(const QString& path, int layerIndex, bool visible, bool autoAdd)
{
  ArcGISTiledLayer* tiledLayer = new ArcGISTiledLayer(QUrl(path), this);
  tiledLayer->setVisible(visible);
  connect(tiledLayer, &ArcGISTiledLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

  if (autoAdd)
  {
    auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
    if (operationalLayers)
      operationalLayers->append(tiledLayer);

    emit layerSelected(tiledLayer);
    Q_UNUSED(layerIndex)
  }
  else
  {
    emit layerCreated(layerIndex, tiledLayer);
  }
}

/*
 \brief Creates an ArcGISVectorTiledLayer with the provided VTPK path.

 \list
   \li \a path - The path to the local data source.
   \li \a layerIndex - The index for which the layer will be added to the operational layer list.
   \li \a visible - Whether the layer should be visible by default.
   \li \a autoAdd - Whether the layer will be automatically added to the operational layer list.
        If \c false, it will not add automatically. Instead, a signal will emit once the Layer has
        been constructed.
 \endlist
*/void AddLocalDataController::createVectorTiledLayer(const QString& path, int layerIndex, bool visible, bool autoAdd)
{
  ArcGISVectorTiledLayer* vectorTiledLayer = new ArcGISVectorTiledLayer(QUrl(path), this);
  vectorTiledLayer->setVisible(visible);
  connect(vectorTiledLayer, &ArcGISVectorTiledLayer::errorOccurred, this, &AddLocalDataController::errorOccurred);

  if (autoAdd)
  {
    auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
    if (operationalLayers)
      operationalLayers->append(vectorTiledLayer);

    emit layerSelected(vectorTiledLayer);
    Q_UNUSED(layerIndex)
  }
  else
  {
    emit layerCreated(layerIndex, vectorTiledLayer);
  }
}

/*
 \brief Returns the tool's name.
*/
QString AddLocalDataController::toolName() const
{
  return QStringLiteral("add local data");
}

/*
 \brief Sets \a properties, such as the directories to search for local data.
*/
void AddLocalDataController::setProperties(const QVariantMap& properties)
{
  const QStringList filePaths = properties[LOCAL_DATAPATHS_PROPERTYNAME].toStringList();
  if (filePaths.empty())
    return;

  for (const QString& filePath : filePaths)
    addPathToDirectoryList(filePath);

  refreshLocalDataModel();
}
