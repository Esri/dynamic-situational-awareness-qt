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

// Toolkit
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "TableOfContentsController.h"
#include "AbstractTool.h"

// Qt
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

using namespace Esri::ArcGISRuntime;

/*
 \brief Constructor that takes an optional \a parent.
 */
LayerCacheManager::LayerCacheManager(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  TableOfContentsController* toc = Toolkit::ToolManager::instance().tool<TableOfContentsController>();
  if (toc)
  {
    connect(toc, &TableOfContentsController::layerListModelChanged, this, &LayerCacheManager::onLayerListChanged);
    connect(toc, &TableOfContentsController::layerChanged, this, &LayerCacheManager::onLayerChanged);
  }
}

/*
 \brief Destructor
 */
LayerCacheManager::~LayerCacheManager()
{
}

/*
 \brief Returns the Tool Name
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

}

/*
 \brief Updates the layer list cache with the provided \a layer.
 */
void LayerCacheManager::onLayerChanged(Layer* layer)
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
  layerJson.insert("path", QString(layerPath).simplified());
  layerJson.insert("visible", layer->isVisible() ? "true" : "false");
  QJsonDocument layerJsonDoc(layerJson);
  m_layers.append(QString(layerJsonDoc.toJson(QJsonDocument::Compact)));

  emit propertyChanged("Layers", m_layers);
}

/*
 \brief Resets the layer string list.
*/
void LayerCacheManager::onLayerListChanged()
{
  m_layers.clear();
}
