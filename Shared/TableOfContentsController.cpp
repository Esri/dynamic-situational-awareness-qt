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

#include "GeoView.h"
#include "LayerListModel.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "DsaUtility.h"
#include "TableOfContentsController.h"

using namespace Esri::ArcGISRuntime;

TableOfContentsController::TableOfContentsController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolManager::instance().resourceProvider(), &Toolkit::ToolResourceProvider::mapChanged,
          this, &TableOfContentsController::updateLayerListModel);
  connect(Toolkit::ToolManager::instance().resourceProvider(), &Toolkit::ToolResourceProvider::sceneChanged,
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

  GeoView* geoView = Toolkit::ToolManager::instance().resourceProvider()->geoView();
  if (!geoView)
    return;

  geoView->setViewpoint(Viewpoint(layer->fullExtent()));
}

QString TableOfContentsController::toolName() const
{
  return QStringLiteral("table of contents");
}

void TableOfContentsController::updateLayerListModel()
{
  auto operationalLayers = Toolkit::ToolManager::instance().resourceProvider()->operationalLayers();
  if (operationalLayers)
  {
    m_layerListModel = operationalLayers;
    emit layerListModelChanged();
  }
}
