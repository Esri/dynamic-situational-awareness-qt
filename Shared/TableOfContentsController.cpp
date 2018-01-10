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

#include <QFileInfo>

#include "GeoView.h"
#include "FeatureLayer.h"
#include "LayerListModel.h"
#include "RasterLayer.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "DsaUtility.h"
#include "TableOfContentsController.h"

using namespace Esri::ArcGISRuntime;

TableOfContentsController::TableOfContentsController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mapChanged,
          this, &TableOfContentsController::updateLayerListModel);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::sceneChanged,
          this, &TableOfContentsController::updateLayerListModel);
  updateLayerListModel();
}

TableOfContentsController::~TableOfContentsController()
{
}

QAbstractListModel* TableOfContentsController::layerListModel() const
{
  return m_layerListModel;
}

void TableOfContentsController::zoomTo(int layerIndex)
{
  if (!m_layerListModel)
    return;

  if (layerIndex >= m_layerListModel->rowCount())
    return;

  Layer* layer = m_layerListModel->at(layerIndex);
  if (!layer)
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  geoView->setViewpoint(Viewpoint(layer->fullExtent()));
}

void TableOfContentsController::removeAt(int layerIndex)
{
  if (!m_layerListModel)
    return;

  m_layerListModel->removeAt(layerIndex);
}

void TableOfContentsController::moveUp(int layerIndex)
{
  if (!m_layerListModel)
    return;

  if (layerIndex <= 0)
    return;

  m_layerListModel->move(layerIndex, layerIndex - 1);
}

void TableOfContentsController::moveDown(int layerIndex)
{
  if (!m_layerListModel)
    return;

  if (layerIndex >= m_layerListModel->rowCount())
    return;

  m_layerListModel->move(layerIndex, layerIndex + 1);
}

void TableOfContentsController::moveFromTo(int fromIndex, int toIndex)
{
  if (!m_layerListModel)
    return;

  m_layerListModel->move(fromIndex, toIndex);
}

QString TableOfContentsController::alternateName(int layerIndex)
{
  const QString unknownName("????");
  if (!m_layerListModel)
    return QString(unknownName);

  if (layerIndex >= m_layerListModel->rowCount())
    return QString(unknownName);

  Layer* layer = m_layerListModel->at(layerIndex);
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

TableOfContentsController::LayerGeometryType TableOfContentsController::layerGeometryType(int layerIndex)
{
  if (!m_layerListModel)
    return LayerGeometryType::Unknown;

  if (layerIndex >= m_layerListModel->rowCount())
    return LayerGeometryType::Unknown;

  Layer* layer = m_layerListModel->at(layerIndex);
  if (!layer)
    return LayerGeometryType::Unknown;

  if (layer->loadStatus() != LoadStatus::Loaded && layer->loadStatus() != LoadStatus::FailedToLoad)
  {
    if (!m_layerConnections.contains(layer))
      m_layerConnections.insert(layer, connect(layer, &Layer::doneLoading, this, [this, layer](const Error& loadError)
      {
        m_layerConnections.remove(layer);

        if (!loadError.isEmpty())
          emit errorOccurred(loadError);
        else
          emit layerListModelChanged();
      }));
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
  default:
    return LayerGeometryType::Unknown;
  }
}

QString TableOfContentsController::toolName() const
{
  return QStringLiteral("table of contents");
}

void TableOfContentsController::updateLayerListModel()
{
  auto operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
  {
    m_layerListModel = operationalLayers;
    emit layerListModelChanged();
  }
}
