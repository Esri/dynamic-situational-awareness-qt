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

#include "AbstractAlert.h"
#include "AlertListModel.h"
#include "EditAlertsController.h"
#include "FeatureOverlayManager.h"
#include "GraphicsOverlayManager.h"
#include "ProximityPairAlert.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "GeoView.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "FeatureLayer.h"
#include "Layer.h"
#include "LayerListModel.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

EditAlertsController::EditAlertsController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_layerNames(new QStringListModel(this)),
  m_statusNames(new QStringListModel(QStringList{"Low", "Moderate", "High", "Critical"},this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged,
          this, &EditAlertsController::onGeoviewChanged);

  connect(AlertListModel::instance(), &AlertListModel::dataChanged, this, &EditAlertsController::conditionsListChanged);

  onGeoviewChanged();
}

EditAlertsController::~EditAlertsController()
{
}

QString EditAlertsController::toolName() const
{
  return "Edit Alerts";
}

void EditAlertsController::addWithinDistanceAlert(int statusIndex, int sourceLayerIndex, double distance, int itemId, int targetLayerIndex)
{
  if (statusIndex < 0 ||
      sourceLayerIndex < 0 ||
      distance < 0.0 ||
      itemId < 0 ||
      targetLayerIndex < 0)
    return;

  if (sourceLayerIndex == targetLayerIndex)
    return;

  AlertStatus status = static_cast<AlertStatus>(statusIndex);
  if (status > AlertStatus::Critical)
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  AbstractOverlayManager* sourceOverlayMgr = nullptr;
  AbstractOverlayManager* targetOverlayMgr = nullptr;
  int currIndex = -1;
  LayerListModel* operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
  {
    const int opLayersCount = operationalLayers->rowCount();
    for (int i = 0; i < opLayersCount; ++i)
    {
      currIndex++;
      Layer* layer = operationalLayers->at(i);
      if (!layer)
        continue;

      FeatureLayer* featLayer = qobject_cast<FeatureLayer*>(layer);
      if (!featLayer)
        continue;

      if (currIndex == sourceLayerIndex)
        sourceOverlayMgr = new FeatureOverlayManager(featLayer, this);

      if (currIndex == targetLayerIndex)
        targetOverlayMgr = new FeatureOverlayManager(featLayer, this);
    }
  }

  GraphicsOverlayListModel* graphicsOverlays = geoView->graphicsOverlays();
  if (graphicsOverlays)
  {
    const int overlaysCount = graphicsOverlays->rowCount();
    for (int i = 0; i < overlaysCount; ++i)
    {
      ++currIndex;
      GraphicsOverlay* overlay = graphicsOverlays->at(i);
      if (!overlay)
        continue;

      if (currIndex == sourceLayerIndex)
        sourceOverlayMgr = new GraphicsOverlayManager(overlay, this);

      if (currIndex == targetLayerIndex)
        targetOverlayMgr = new GraphicsOverlayManager(overlay, this);
    }
  }

  if (!targetOverlayMgr && !sourceOverlayMgr)
    return;

  GeoElement* targetElement = targetOverlayMgr->elementAt(itemId);
  if (!targetElement)
    return;


  auto createProximityAlert = [this, targetElement, distance, status, sourceOverlayMgr, targetOverlayMgr](int newElement)
  {
    GeoElement* sourceElement = sourceOverlayMgr->elementAt(newElement);
    if (!sourceElement)
      return;

    ProximityPairAlert* geofenceAlert = new ProximityPairAlert(sourceElement,
                                                               targetElement,
                                                               sourceOverlayMgr,
                                                               targetOverlayMgr,
                                                               distance,
                                                               this);
    geofenceAlert->setStatus(status);
    geofenceAlert->setMessage("Location in geofence");
    geofenceAlert->setViewed(false);
    AlertListModel::instance()->addAlert(geofenceAlert);
  };

  const int totalElements = static_cast<int>(sourceOverlayMgr->numberOfElements());
  for (qint64 i = 0; i < totalElements; ++i)
    createProximityAlert(i);

  connect(sourceOverlayMgr, &AbstractOverlayManager::elementAdded, this, createProximityAlert);
}

void EditAlertsController::removeConditionAt(int rowIndex)
{
  AlertListModel::instance()->removeAt(rowIndex);
}

void EditAlertsController::togglePickMode()
{
  m_pickMode = !m_pickMode;
  emit pickModeChanged();
}

QAbstractItemModel* EditAlertsController::layerNames() const
{
  return m_layerNames;
}

QAbstractItemModel* EditAlertsController::statusNames() const
{
  return m_statusNames;
}

QAbstractItemModel* EditAlertsController::conditionsList() const
{
  return AlertListModel::instance();
}

bool EditAlertsController::pickMode() const
{
  return m_pickMode;
}

void EditAlertsController::onGeoviewChanged()
{
  setLayerNames(QStringList());

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  LayerListModel* operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
  {
    connect(operationalLayers, &LayerListModel::layerAdded, this, &EditAlertsController::onLayersChanged);
    connect(operationalLayers, &LayerListModel::layerRemoved, this, &EditAlertsController::onLayersChanged);
  }

  GraphicsOverlayListModel* graphicsOverlays = geoView->graphicsOverlays();
  if (graphicsOverlays)
  {
    connect(graphicsOverlays, &GraphicsOverlayListModel::graphicsOverlayAdded, this, &EditAlertsController::onLayersChanged);
    connect(graphicsOverlays, &GraphicsOverlayListModel::graphicsOverlayRemoved, this, &EditAlertsController::onLayersChanged);
  }

  onLayersChanged();
}

void EditAlertsController::onLayersChanged()
{
  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
  {
    setLayerNames(QStringList());
    return;
  }

  QStringList newList;
  LayerListModel* operationalLayers = Toolkit::ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
  {
    const int opLayersCount = operationalLayers->rowCount();
    for (int i = 0; i < opLayersCount; ++i)
    {
      Layer* lyr = operationalLayers->at(i);
      if (!lyr)
        continue;

      FeatureLayer* featLayer = qobject_cast<FeatureLayer*>(lyr);
      if (!featLayer)
        continue;

      if (featLayer->loadStatus() != LoadStatus::Loaded)
        connect(featLayer, &FeatureLayer::doneLoading, this, &EditAlertsController::onLayersChanged);
      else
        newList.append(featLayer->name());
    }
  }

  GraphicsOverlayListModel* graphicsOverlays = geoView->graphicsOverlays();
  if (graphicsOverlays)
  {
    const int overlaysCount = graphicsOverlays->rowCount();
    for (int i = 0; i < overlaysCount; ++i)
    {
      GraphicsOverlay* overlay = graphicsOverlays->at(i);
      if (!overlay)
        continue;

      newList.append(overlay->overlayId());
    }
  }

  setLayerNames(newList);
}

void EditAlertsController::setLayerNames(const QStringList& layerNames)
{
  const QStringList existingNames = m_layerNames->stringList();
  if (existingNames == layerNames)
    return;

  m_layerNames->setStringList(layerNames);
  emit layerNamesChanged();
}

