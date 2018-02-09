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

  //  Scene* scene = Toolkit::ToolResourceProvider::instance()->scene();
  //  connect(scene, &Scene::doneLoading, this, [this](Error e)
  //  {
  //    //qDebug() << "Done Loading";
  //  });

  //qDebug() << "adding stuff";
 // qDebug() << properties[LAYERS_PROPERTYNAME];
  const QVariant layersData = properties.value(LAYERS_PROPERTYNAME);
  if (layersData.isNull())
    return;

  const auto layersList = layersData.toList();
  if (layersList.isEmpty())
    return;

  QJsonArray layerJsonArray = QJsonArray::fromVariantList(layersList);
  if (layerJsonArray.isEmpty())
    return;

  qDebug() << layerJsonArray.count();

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

    m_localDataController->addLayerFromPath(jsonObject.value(layerPathKey).toString());
    //qDebug() << jsonObject.value(layerPathKey) << jsonObject.value(layerVisibleKey);
  };
  //  QString layerList = properties[LAYERS_PROPERTYNAME];
  //  QJsonDocument jsonString = QJsonDocument::fromJson(layerList.toUtf8());
  //  qDebug() << properties[LAYERS_PROPERTYNAME];

  //  for (QString layerString : layerList)
  //  {
  ////    qDebug() << layerString;
  //    QJsonObject layerJson = QJsonDocument::fromJson(layerString.toUtf8()).object();
  ////    if (!layerJson.contains("path") || !layerJson.contains("visible"))
  ////      continue;
  ////    qDebug() << layerJson.value("path").toString();
  //    m_localDataController->addLayerFromPath(layerJson.value("path").toString());
  //  }
  m_initialLoadCompleted = true;
}

/*
 \brief Updates the layer list cache with the provided \a layer.
 */
void LayerCacheManager::layerToJson(Layer* layer)
{
  QString layerPath;

  // Get Feature Layers
  auto featureLayer = dynamic_cast<FeatureLayer*>(layer);
  if (featureLayer)
  {
    // Check if a Geodatabase
    auto gdbFeatureTable = dynamic_cast<GeodatabaseFeatureTable*>(featureLayer->featureTable());
    if (gdbFeatureTable)
      layerPath = gdbFeatureTable->geodatabase()->path();

    // Check if a GeoPackage
    auto gpkgFeatureTable = dynamic_cast<GeoPackageFeatureTable*>(featureLayer->featureTable());
    if (gpkgFeatureTable)
      layerPath = gpkgFeatureTable->geoPackage()->path();

    // Check if a Shapefile
    auto shpFeatureTable = dynamic_cast<ShapefileFeatureTable*>(featureLayer->featureTable());
    if (shpFeatureTable)
      layerPath = shpFeatureTable->path();
  }

  // Get Raster Layers
  auto rasterLayer = dynamic_cast<RasterLayer*>(layer);
  if (rasterLayer)
  {
    // Check if a GeoPackage
    auto gpkgRaster = dynamic_cast<GeoPackageRaster*>(rasterLayer->raster());
    if (gpkgRaster)
      layerPath = gpkgRaster->geoPackage()->path();

    // Check if a Raster
    auto raster = dynamic_cast<Raster*>(rasterLayer->raster());
    if (raster)
      layerPath = raster->path();
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
