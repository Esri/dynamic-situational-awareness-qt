
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

// PCH header
#include "pch.hpp"

#include "LayerCacheManager.h"

// example app headers
#include "AddLocalDataController.h"
#include "OpenMobileScenePackageController.h"
#include "MarkupLayer.h"

// toolkit headers
#include "AbstractTool.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "ArcGISSceneLayer.h"
#include "ArcGISTiledLayer.h"
#include "ArcGISVectorTiledLayer.h"
#include "FeatureLayer.h"
#include "FeatureTable.h"
#include "GeoPackage.h"
#include "GeoPackageFeatureTable.h"
#include "GeoPackageRaster.h"
#include "Geodatabase.h"
#include "GeodatabaseFeatureTable.h"
#include "KmlLayer.h"
#include "Raster.h"
#include "RasterLayer.h"
#include "Scene.h"
#include "ShapefileFeatureTable.h"

// Qt headers
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

namespace Dsa {

const QString LayerCacheManager::LAYERS_PROPERTYNAME = "Layers";
const QString LayerCacheManager::ELEVATION_PROPERTYNAME = "DefaultElevationSource";
const QString LayerCacheManager::layerPathKey = "path";
const QString LayerCacheManager::layerVisibleKey = "visible";
const QString LayerCacheManager::layerTypeKey = "type";
const QString LayerCacheManager::layerIdKey = "id";
const QString LayerCacheManager::layerTypeFeatureLayerGeodatabase = "FeatureLayerGeodatabase";
const QString LayerCacheManager::layerTypeFeatureLayerGeoPackage = "FeatureLayerGeoPackage";
const QString LayerCacheManager::layerTypeRasterLayerGeoPackage = "RasterLayerGeoPackage";

using namespace Esri::ArcGISRuntime;

/*!
  \class Dsa::LayerCacheManager
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Tool controller responsible for managing the layers in the app.
 */

/*!
 \brief Constructor that takes an optional \a parent.
 */
LayerCacheManager::LayerCacheManager(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  // obtain Add Local Data Controller
  m_localDataController = Toolkit::ToolManager::instance().tool<AddLocalDataController>();

  // connect to new scene for cases where a new scene is set on the SceneView (via MobileScenePackage)
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::sceneChanged, this, [this]()
  {
    m_scene = Toolkit::ToolResourceProvider::instance()->scene();
    m_layers = QJsonArray();
    m_inputLayerJsonArray = QJsonArray();
    m_initialLayerCache.clear();
    connectSignals();

    // only add initial layers on initial load. Once the user selects a new scene, the layer list will be cleared
    auto mobileSceneTool = Toolkit::ToolManager::instance().tool<OpenMobileScenePackageController>();
    if (mobileSceneTool->userSelected())
      return;

    // If this is the initial load of a MSPK Scene, add additional layers
    const QVariant layersData = m_initialSettings.value(LAYERS_PROPERTYNAME);
    const auto layersList = layersData.toList();
    m_inputLayerJsonArray = QJsonArray::fromVariantList(layersList);

    auto it = m_inputLayerJsonArray.constBegin();
    auto itEnd = m_inputLayerJsonArray.constEnd();
    int layerIndex = 0;
    for (; it != itEnd; ++it)
    {
      const QJsonValue jsonVal = *it;
      if (jsonVal.isNull())
        continue;

      const QJsonObject jsonObject = jsonVal.toObject();
      if (jsonObject.isEmpty())
        continue;

      jsonToLayer(jsonObject, layerIndex);

      layerIndex++;
    };
  });

  // connect to the initial default scene created in code
  connectSignals();

  Toolkit::ToolManager::instance().addTool(this);
}

/*!
 \brief Destructor
 */
LayerCacheManager::~LayerCacheManager()
{
}

/*!
 \brief Returns the tool's name
 */
QString LayerCacheManager::toolName() const
{
  return QStringLiteral("Layer Cache Manager");
}

/*!
 \brief Sets \a properties from the configuration file
 */
void LayerCacheManager::setProperties(const QVariantMap& properties)
{
  if (m_initialLoadCompleted || !m_localDataController)
    return;

  m_initialSettings = properties;
  const QVariant layersData = properties.value(LAYERS_PROPERTYNAME);
  const auto layersList = layersData.toList();
  m_inputLayerJsonArray = QJsonArray::fromVariantList(layersList);

  auto it = m_inputLayerJsonArray.constBegin();
  auto itEnd = m_inputLayerJsonArray.constEnd();
  int layerIndex = 0;
  for (; it != itEnd; ++it)
  {
    const QJsonValue jsonVal = *it;
    if (jsonVal.isNull())
      continue;

    const QJsonObject jsonObject = jsonVal.toObject();
    if (jsonObject.isEmpty())
      continue;

    jsonToLayer(jsonObject, layerIndex);

    layerIndex++;
  };

  // Add the default elevation source
  const QVariant elevationData = properties.value(ELEVATION_PROPERTYNAME);
  const QStringList pathList = elevationData.toStringList();

  // If size is 1, it could be a TPK or raster
  if (pathList.length() == 1)
  {
    // Get the string
    const QString elevationSource = pathList.at(0);
    QFileInfo elevationSourceInfo(elevationSource);

    // Check if TPK or not
    if (elevationSourceInfo.suffix().toLower() == "tpk")
      m_localDataController->createElevationSourceFromTpk(elevationSource);
    else
      m_localDataController->createElevationSourceFromRasters(QStringList{elevationSource});
  }
  // If more than 1, it is a list of rasters
  else if (pathList.length() > 1)
  {
    m_localDataController->createElevationSourceFromRasters(pathList);
  }

  m_initialLoadCompleted = true;
}

/*!
 \brief Creates a Layer from the provided \a jsonObject and adds at the given \a layerIndex.

  Obtain the output Layer through the \l jsonToLayerCompleted() signal.
*/
void LayerCacheManager::jsonToLayer(const QJsonObject& jsonObject, const int layerIndex)
{
  if (!m_localDataController)
    return;

  const QString layerType = jsonObject.value(layerTypeKey).toString();
  const QString layerPath = jsonObject.value(layerPathKey).toString();
  const bool layerVisible = jsonObject.value(layerVisibleKey).toString() == "true";
  const int layerId = jsonObject.value(layerIdKey).toString().toInt();

  if (layerType.isEmpty())
    m_localDataController->addLayerFromPath(layerPath, layerIndex, layerVisible, false);
  else if (layerType == layerTypeFeatureLayerGeodatabase)
    m_localDataController->createFeatureLayerGeodatabaseWithId(layerPath, layerIndex, layerId, layerVisible, false);
  else if (layerType == layerTypeFeatureLayerGeoPackage)
    m_localDataController->createFeatureLayerGeoPackage(layerPath, layerIndex, layerId, layerVisible, false);
  else if (layerType == layerTypeRasterLayerGeoPackage)
    m_localDataController->createRasterLayerGeoPackage(layerPath, layerIndex, layerId, layerVisible, false);
}

/*!
 \brief Updates the layer list cache with the provided \a layer.

 Obtain the updated JSON from \l layerJson() after layerJsonChanged() emits.
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
      layerId = QString::number(gdbFeatureTable->serviceLayerId());
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
    layerPath = sceneLayer->url().toLocalFile();

  // Get TPKs
  auto tiledLayer = dynamic_cast<ArcGISTiledLayer*>(layer);
  if (tiledLayer)
    layerPath = tiledLayer->tileCache() ? tiledLayer->tileCache()->path() : tiledLayer->url().toLocalFile();

  // Get VTPKs
  auto vectorTiledLayer = dynamic_cast<ArcGISVectorTiledLayer*>(layer);
  if (vectorTiledLayer)
    layerPath = vectorTiledLayer->vectorTileCache() ? vectorTiledLayer->vectorTileCache()->path() : vectorTiledLayer->url().toString();

  // Get Markups
  auto markupLayer = dynamic_cast<MarkupLayer*>(layer);
  if (markupLayer)
    layerPath = markupLayer->path();

  // Get KmlLayers
  auto kmlLayer = dynamic_cast<KmlLayer*>(layer);
  if (kmlLayer)
  {
    const QUrl kmlUrl = kmlLayer->dataset()->url();
    layerPath = kmlUrl.isLocalFile() ? kmlUrl.toLocalFile() : kmlUrl.toString();
  }

  // Don't serialize invalid layers
  if (layerPath.isEmpty())
  {
    return;
  }

  // Don't serialize data in excluded path locations
  for (const auto& excludedPath : m_excludedPaths)
  {
    if (layerPath.startsWith(excludedPath))
      return;
  }

  // add the layer to the layer list for caching
  QJsonObject layerJson;
  layerJson.insert(layerPathKey, QString(layerPath).simplified());
  layerJson.insert(layerVisibleKey, layer->isVisible() ? "true" : "false");
  if (layerId.length() > 0)
    layerJson.insert(layerIdKey, layerId);
  if (layerType.length() > 0)
    layerJson.insert(layerTypeKey, layerType);

  m_layers.append(layerJson);

  emit layerJsonChanged();
}

/*!
 \brief Resets and recreates the layer JSON array and writes it to the app properties.
*/
void LayerCacheManager::onLayerListChanged()
{
  m_scene = Toolkit::ToolResourceProvider::instance()->scene();
  if (!m_initialLoadCompleted)
    return;

  // clear the JSON
  m_layers = QJsonArray();

  const auto operationalLayers = m_scene->operationalLayers();
  if (!operationalLayers)
    return;

  // update the JSON array
  const int count = operationalLayers->size();
  for (int i = 0; i < count; i++)
  {
    layerToJson(operationalLayers->at(i));
  }

  // write to the config file
  emit propertyChanged(LAYERS_PROPERTYNAME, m_layers.toVariantList());
}

/*!
 \brief Returns the LayerCacheManager's list of layers as a JSON array.
*/
QJsonArray LayerCacheManager::layerJson() const
{
  return m_layers;
}

void LayerCacheManager::addExcludedPath(const QString& exludedPath)
{
  qDebug() << "add exlcuded path";
  m_excludedPaths.append(exludedPath);
}

void LayerCacheManager::connectSignals()
{
  // obtain Scene and connect slot
  m_scene = Toolkit::ToolResourceProvider::instance()->scene();
  if (m_scene)
  {
    // disconnect signals
    if (m_dataChangedConnection)
      disconnect(m_dataChangedConnection);

    if (m_layerAddedConnection)
      disconnect(m_layerAddedConnection);

    if (m_layerRemovedConnection)
      disconnect(m_layerRemovedConnection);

    if (m_layoutChangedConnection)
      disconnect(m_layoutChangedConnection);

    if (m_modelResetConnection)
      disconnect(m_modelResetConnection);

    // connect signals
    m_dataChangedConnection = connect(m_scene->operationalLayers(), &LayerListModel::dataChanged, this, &LayerCacheManager::onLayerListChanged); // layer objects have been added or changed
    m_layerAddedConnection = connect(m_scene->operationalLayers(), &LayerListModel::layerAdded, this, &LayerCacheManager::onLayerListChanged); // layer objects have been added
    m_layerRemovedConnection = connect(m_scene->operationalLayers(), &LayerListModel::layerRemoved, this, &LayerCacheManager::onLayerListChanged); // layer has been removed
    m_layoutChangedConnection = connect(m_scene->operationalLayers(), &LayerListModel::layoutChanged, this, &LayerCacheManager::onLayerListChanged); // order changed
    m_modelResetConnection = connect(m_scene->operationalLayers(), &LayerListModel::modelReset, this, &LayerCacheManager::onLayerListChanged); // order changed
  }

  if (m_localDataController)
  {
    // disconnect
    if (m_layerCreatedConnection)
      disconnect(m_layerCreatedConnection);

    m_initialLayerCache.clear();

    // cache the created layers
    m_layerCreatedConnection = connect(m_localDataController, &AddLocalDataController::layerCreated, this, [this](int layerIndex, Layer* layer)
    {
      m_scene = Toolkit::ToolResourceProvider::instance()->scene();
      m_initialLayerCache.insert(layerIndex, layer);
      const int layerCount = m_inputLayerJsonArray.size();
      emit jsonToLayerCompleted(layer);

      // once all the layers are created, add them in the proper order
      if (m_initialLayerCache.size() == layerCount)
      {
        if (!m_scene)
          return;

        for (int i = 0; i < layerCount; i++)
        {
          m_scene->operationalLayers()->append(m_initialLayerCache.value(i));
        }
      }
    });
  }
}

} // Dsa

// Signal Documentation
/*!
  \fn void LayerCacheManager::layerJsonChanged();
  \brief Signal emitted when the layer JSON changes.
 */

/*!
  \fn void LayerCacheManager::jsonToLayerCompleted(Esri::ArcGISRuntime::Layer* layer);;
  \brief Signal emitted when the deserialization from JSON to Layer completes.

  The resulting \a layer is passed through as a parameter.
 */

