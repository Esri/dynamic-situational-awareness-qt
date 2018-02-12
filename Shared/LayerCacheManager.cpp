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

#include "LayerCacheManager.h"

// Runtime API
#include "FeatureLayer.h"
#include "RasterLayer.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "FeatureTable.h"
#include "GeoPackage.h"
#include "GeoPackageFeatureTable.h"
#include "GeoPackageRaster.h"
#include "ShapefileFeatureTable.h"
#include "ArcGISSceneLayer.h"
#include "ArcGISTiledLayer.h"
#include "ArcGISVectorTiledLayer.h"
#include "Raster.h"
#include "Scene.h"

// Toolkit
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "AbstractTool.h"

// DSA
#include "AddLocalDataController.h"

// Qt
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

const QString LayerCacheManager::LAYERS_PROPERTYNAME = "Layers";
const QString LayerCacheManager::layerPathKey = "path";
const QString LayerCacheManager::layerVisibleKey = "visible";
const QString LayerCacheManager::layerTypeKey = "type";
const QString LayerCacheManager::layerIdKey = "id";
const QString LayerCacheManager::layerTypeFeatureLayerGeodatabase = "FeatureLayerGeodatabase";
const QString LayerCacheManager::layerTypeFeatureLayerGeoPackage = "FeatureLayerGeoPackage";
const QString LayerCacheManager::layerTypeRasterLayerGeoPackage = "RasterLayerGeoPackage";

using namespace Esri::ArcGISRuntime;

/*
 \brief Constructor that takes an optional \a parent.
 */
LayerCacheManager::LayerCacheManager(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  // obtain Add Local Data Controller
  m_localDataController = Toolkit::ToolManager::instance().tool<AddLocalDataController>();

  // obtain Scene and connect slot
  m_scene = Toolkit::ToolResourceProvider::instance()->scene();
  if (m_scene)
  {
    connect(m_scene->operationalLayers(), &LayerListModel::dataChanged, this, &LayerCacheManager::onLayerListChanged); // layer objects have been added or changed
    connect(m_scene->operationalLayers(), &LayerListModel::layerRemoved, this, &LayerCacheManager::onLayerListChanged); // layer has been removed
    connect(m_scene->operationalLayers(), &LayerListModel::layoutChanged, this, &LayerCacheManager::onLayerListChanged); // order changed
    connect(m_scene->operationalLayers(), &LayerListModel::modelReset, this, &LayerCacheManager::onLayerListChanged); // order changed
  }
}

/*
 \brief Destructor
 */
LayerCacheManager::~LayerCacheManager()
{
}

/*
 \brief Returns the tool name
 */
QString LayerCacheManager::toolName() const
{
  return QStringLiteral("Layer Cache Manager");
}


/*
 \brief Set \a properties from the configuration file
 */
void LayerCacheManager::setProperties(const QVariantMap& properties)
{
  if (!m_localDataController)
    return;

  if (m_initialLoadCompleted)
    return;

  const QVariant layersData = properties.value(LAYERS_PROPERTYNAME);
  if (layersData.isNull())
    return;

  const auto layersList = layersData.toList();
  if (layersList.isEmpty())
    return;

  QJsonArray layerJsonArray = QJsonArray::fromVariantList(layersList);
  if (layerJsonArray.isEmpty())
    return;

  auto it = layerJsonArray.constBegin();
  auto itEnd = layerJsonArray.constEnd();
  for (; it != itEnd; ++it)
  {
    const QJsonValue jsonVal = *it;
    if (jsonVal.isNull())
      continue;

    const QJsonObject jsonObject = jsonVal.toObject();
    if (jsonObject.isEmpty())
      continue;

    const QString layerType = jsonObject.value(layerTypeKey).toString();
    const QString layerPath = jsonObject.value(layerPathKey).toString();
    const bool layerVisible = jsonObject.value(layerVisibleKey).toString() == "true";
    const int layerId = jsonObject.value(layerIdKey).toString().toInt();

    if (layerType.isEmpty())
      m_localDataController->addLayerFromPath(layerPath, layerVisible);
    else if (layerType == layerTypeFeatureLayerGeodatabase)
      m_localDataController->createFeatureLayerGeodatabase(layerPath, layerId, layerVisible);
    else if (layerType == layerTypeFeatureLayerGeoPackage)
      m_localDataController->createFeatureLayerGeoPackage(layerPath, layerId, layerVisible);
    else if (layerType == layerTypeRasterLayerGeoPackage)
      m_localDataController->createRasterLayerGeoPackage(layerPath, layerId, layerVisible);

  };

  m_initialLoadCompleted = true;
}

/*
 \brief Updates the layer list cache with the provided \a layer.
 */
void LayerCacheManager::layerToJson(Layer* layer)
{
  QString layerPath;
  QString layerType;
  QString layerId;

  // Get Feature Layers
  auto featureLayer = dynamic_cast<FeatureLayer*>(layer);
  if (featureLayer)
  {
    // Check if a Geodatabase
    auto gdbFeatureTable = dynamic_cast<GeodatabaseFeatureTable*>(featureLayer->featureTable());
    if (gdbFeatureTable)
    {
      layerPath = gdbFeatureTable->geodatabase()->path();
      layerType = layerTypeFeatureLayerGeodatabase;
      layerId = QString::number(gdbFeatureTable->geodatabase()->geodatabaseFeatureTables().indexOf(gdbFeatureTable));
    }

    // Check if a GeoPackage
    auto gpkgFeatureTable = dynamic_cast<GeoPackageFeatureTable*>(featureLayer->featureTable());
    if (gpkgFeatureTable)
    {
      layerPath = gpkgFeatureTable->geoPackage()->path();
      layerType = layerTypeFeatureLayerGeoPackage;
      layerId = QString::number(gpkgFeatureTable->geoPackage()->geoPackageFeatureTables().indexOf(gpkgFeatureTable));
    }

    // Check if a Shapefile
    auto shpFeatureTable = dynamic_cast<ShapefileFeatureTable*>(featureLayer->featureTable());
    if (shpFeatureTable)
      layerPath = shpFeatureTable->path();
  }

  // Get Raster Layers
  auto rasterLayer = dynamic_cast<RasterLayer*>(layer);
  if (rasterLayer)
  {
    // Check if a Raster
    auto raster = dynamic_cast<Raster*>(rasterLayer->raster());
    if (raster)
      layerPath = raster->path();

    // Check if a GeoPackage
    auto gpkgRaster = dynamic_cast<GeoPackageRaster*>(rasterLayer->raster());
    if (gpkgRaster)
    {
      layerPath = gpkgRaster->geoPackage()->path();
      layerType = layerTypeRasterLayerGeoPackage;
      layerId = QString::number(gpkgRaster->geoPackage()->geoPackageRasters().indexOf(gpkgRaster));
    }
  }

  // Get Scene Layers
  auto sceneLayer = dynamic_cast<ArcGISSceneLayer*>(layer);
  if (sceneLayer)
    layerPath = sceneLayer->url().toString();


  // Get TPKs
  auto tiledLayer = dynamic_cast<ArcGISTiledLayer*>(layer);
  if (tiledLayer)
    layerPath = tiledLayer->url().toString();

  // Get VTPKs
  auto vectorTiledLayer = dynamic_cast<ArcGISVectorTiledLayer*>(layer);
  if (vectorTiledLayer)
    layerPath = vectorTiledLayer->url().toString();

  // add the layer to the layer list for caching
  QJsonObject layerJson;
  layerJson.insert(layerPathKey, QString(layerPath).simplified());
  layerJson.insert(layerVisibleKey, layer->isVisible() ? "true" : "false");
  if (layerId.length() > 0)
    layerJson.insert(layerIdKey, layerId);
  if (layerType.length() > 0)
    layerJson.insert(layerTypeKey, layerType);

  m_layers.append(layerJson);
}

/*
 \brief Resets and recreates the layer JSON array and writes it to the app properties.
*/
void LayerCacheManager::onLayerListChanged()
{
  // clear the JSON
  m_layers = QJsonArray();

  // update the JSON array
  for (int i = 0; i < m_scene->operationalLayers()->size(); i++)
  {
    layerToJson(m_scene->operationalLayers()->at(i));
  }

  // write to the config file
  emit propertyChanged(LAYERS_PROPERTYNAME, m_layers.toVariantList());
}
