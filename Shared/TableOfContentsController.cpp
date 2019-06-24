
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

#include "TableOfContentsController.h"

// example app headers
#include "DrawOrderLayerListModel.h"
#include "DsaUtility.h"
#include "MarkupLayer.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "FeatureLayer.h"
#include "FeatureCollectionLayer.h"
#include "GeoView.h"
#include "LayerListModel.h"
#include "RasterLayer.h"

// Qt headers
#include <QFileInfo>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::TableOfContentsController
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Tool controller for managing the table of contents for operational layers.

  \sa Esri::ArcGISRuntime::LayerListModel
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
TableOfContentsController::TableOfContentsController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mapChanged,
          this, &TableOfContentsController::updateLayerListModel);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::sceneChanged,
          this, &TableOfContentsController::updateLayerListModel);
  updateLayerListModel();

  Toolkit::ToolManager::instance().addTool(this);
}

/*!
  \brief Destructor.
 */
TableOfContentsController::~TableOfContentsController()
{
}

/*!
  \property TableOfContentsController::layerListModel
  \brief Returns the list of operational layers in draw order.
 */
QAbstractItemModel* TableOfContentsController::layerListModel() const
{
  return m_drawOrderModel;
}

/*!
  \brief Zoom to the layer at \a layerIndex in the list.
 */
void TableOfContentsController::zoomTo(int layerIndex)
{
  if (!m_layerListModel)
    return;

  const int modelIndex = mappedIndex(layerIndex);

  if (modelIndex >= m_layerListModel->rowCount())
    return;

  Layer* layer = m_layerListModel->at(modelIndex);
  if (!layer)
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  Envelope extent;

  MarkupLayer* markupLayer = dynamic_cast<MarkupLayer*>(layer);
  if (markupLayer)
    extent = markupLayer->featureCollection()->tables()->at(0)->extent();
  else
    extent = layer->fullExtent();

  geoView->setViewpoint(Viewpoint(extent));
}

/*!
  \brief Remove the layer at \a layerIndex in the list.
 */
void TableOfContentsController::removeAt(int layerIndex)
{
  if (!m_layerListModel)
    return;

  const int modelIndex = mappedIndex(layerIndex);

  m_layerListModel->removeAt(modelIndex);
}

/*!
  \brief Move the layer at \a layerIndex in the list down.
 */
void TableOfContentsController::moveDown(int layerIndex)
{
  if (!m_layerListModel)
    return;

  const int modelIndex = mappedIndex(layerIndex);

  if (modelIndex <= 0)
    return;

  m_layerListModel->move(modelIndex, modelIndex - 1);

  refreshLayerOrder();
}

/*!
  \brief Move the layer at \a layerIndex in the list up.
 */
void TableOfContentsController::moveUp(int layerIndex)
{
  if (!m_layerListModel)
    return;

  const int modelIndex = mappedIndex(layerIndex);

  if (modelIndex >= m_layerListModel->rowCount())
    return;

  m_layerListModel->move(modelIndex, modelIndex + 1);

  refreshLayerOrder();
}

/*!
  \brief Move the layer at \a fromIndex to the new position \a toIndex.
 */
void TableOfContentsController::moveFromTo(int fromIndex, int toIndex)
{
  if (!m_layerListModel)
    return;

  const int modelFromIndex = mappedIndex(fromIndex);
  const int modelToIndex = mappedIndex(toIndex);

  if (modelFromIndex == -1 || modelToIndex == -1)
    return;

  m_layerListModel->move(modelFromIndex, modelToIndex);

  refreshLayerOrder();
}

/*!
  \brief Return an alternate name for the layer at \a layerIndex in the list.
 */
QString TableOfContentsController::alternateName(int layerIndex)
{
  const QString unknownName("????");
  if (!m_layerListModel)
    return QString(unknownName);

  const int modelIndex = mappedIndex(layerIndex);

  if (modelIndex >= m_layerListModel->rowCount())
    return QString(unknownName);

  Layer* layer = m_layerListModel->at(modelIndex);
  if (!layer)
    return QString(unknownName);

  QString layerName = layer->name();
  if (!layerName.isEmpty())
    return layerName;

  RasterLayer* rasterLayer = qobject_cast<RasterLayer*>(layer);
  if (!rasterLayer)
    return QString(unknownName);

  Raster* raster = rasterLayer->raster();
  if (!raster || raster->path().isEmpty())
    return QString(unknownName);

  QFileInfo rasterFile(raster->path());

  return rasterFile.baseName();
}

/*!
  \brief Returns the geometry type of the layer at \a layerIndex in the list.
 */
TableOfContentsController::LayerGeometryType TableOfContentsController::layerGeometryType(int layerIndex)
{
  if (!m_layerListModel)
    return LayerGeometryType::Unknown;

  const int modelIndex = mappedIndex(layerIndex);

  if (modelIndex >= m_layerListModel->rowCount())
    return LayerGeometryType::Unknown;

  Layer* layer = m_layerListModel->at(modelIndex);
  if (!layer)
    return LayerGeometryType::Unknown;

  if (layer->loadStatus() != LoadStatus::Loaded && layer->loadStatus() != LoadStatus::FailedToLoad)
  {
    if (!m_layerConnections.contains(layer))
    {
      m_layerConnections.insert(layer, connect(layer, &Layer::doneLoading, this, [this, layer](const Error& loadError)
      {
        m_layerConnections.remove(layer);

        if (!loadError.isEmpty())
          emit errorOccurred(loadError);
        else
          emit layerListModelChanged();
      }));
    }
  }

  switch (layer->layerType())
  {
  case LayerType::FeatureLayer:
  {
    FeatureLayer* featureLayer = qobject_cast<FeatureLayer*>(layer);
    if (!featureLayer)
      return LayerGeometryType::Unknown;

    FeatureTable* featTable = featureLayer->featureTable();
    if (!featTable)
      return LayerGeometryType::Unknown;

    switch(featTable->geometryType())
    {
    case GeometryType::Point:
    case GeometryType::Multipoint:
      return LayerGeometryType::Points;
    case GeometryType::Polyline:
      return LayerGeometryType::Polylines;
    case GeometryType::Polygon:
      return LayerGeometryType::Polygons;
    case GeometryType::Unknown:
    default:
      return LayerGeometryType::Unknown;
    }
  }
  case LayerType::RasterLayer:
    return LayerGeometryType::Raster;
  case LayerType::FeatureCollectionLayer:
    return LayerGeometryType::FreehandMarkup;
  case LayerType::KMLLayer:
    return LayerGeometryType::Kml;
  case LayerType::ArcGISSceneLayer:
    return LayerGeometryType::SceneLayer;
  case LayerType::PointCloudLayer:
    return LayerGeometryType::SceneLayer;
  case LayerType::IntegratedMeshLayer:
    return LayerGeometryType::SceneLayer;
  case LayerType::ArcGISTiledLayer:
    return LayerGeometryType::Raster;
  default:
    return LayerGeometryType::Unknown;
  }
}

/*!
  \brief Return the name of the tool - \c "table of contents".
 */
QString TableOfContentsController::toolName() const
{
  return QStringLiteral("table of contents");
}

/*!
  \brief Update the list of operational layers used by the app.
 */
void TableOfContentsController::updateLayerListModel()
{
  auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers == m_layerListModel)
    return;

  m_layerListModel = operationalLayers;
  if (m_drawOrderModel)
  {
    delete m_drawOrderModel;
    m_drawOrderModel = nullptr;
  }

  if (m_layerListModel)
  {
    m_drawOrderModel = new DrawOrderLayerListModel(this);
    m_drawOrderModel->setSourceModel(m_layerListModel);
  }

  emit layerListModelChanged();
}

/*!
  \brief Returns the operational layer index of the layer at \a index in the draw order model.
 */
int TableOfContentsController::mappedIndex(int index) const
{
  if (!m_layerListModel || !m_drawOrderModel)
    return -1;

  const QModelIndex sourceIndex = m_drawOrderModel->mapToSource(m_drawOrderModel->index(index, 0));
  return sourceIndex.row();
}

/*!
  \internal
 */
void TableOfContentsController::refreshLayerOrder()
{
  // To avoid a re-ordering issue which affects FeatureCollectionLayers in 3D view
  // these types of layers are removed and re-added at the desired index
  const int layerCount = m_layerListModel->rowCount();
  for (int i = 0; i < layerCount; ++i)
  {
    Layer* layer = m_layerListModel->at(i);
    FeatureCollectionLayer* featCollectionLyr = qobject_cast<FeatureCollectionLayer*>(layer);
    if (!featCollectionLyr)
      continue;

    m_layerListModel->removeAt(i);
    m_layerListModel->insert(i, layer);
  }
}

} // Dsa

// Signal Documentation
/*!
  \fn void TableOfContentsController::layerListModelChanged();
  \brief Signal emitted when the LayerListModel changes.
 */
