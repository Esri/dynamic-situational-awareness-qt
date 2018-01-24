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

#include "AlertConditionData.h"
#include "AlertListModel.h"
#include "EditAlertsController.h"
#include "FeatureOverlayManager.h"
#include "GraphicsOverlayManager.h"
#include "IntersectsPairAlert.h"
#include "ProximityPairAlert.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "GeoView.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "FeatureLayer.h"
#include "Layer.h"
#include "LayerListModel.h"

using namespace Esri::ArcGISRuntime;

struct ResultsManager {

  QList<IdentifyLayerResult*>& m_results;

  ResultsManager(QList<IdentifyLayerResult*>& results):
    m_results(results)
  {
  }

  ~ResultsManager()
  {
    qDeleteAll(m_results);
  }
};

struct GraphicsResultsManager {

  QList<IdentifyGraphicsOverlayResult*>& m_results;

  GraphicsResultsManager(QList<IdentifyGraphicsOverlayResult*>& results):
    m_results(results)
  {
  }

  ~GraphicsResultsManager()
  {
    qDeleteAll(m_results);
  }
};

EditAlertsController::EditAlertsController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_layerNames(new QStringListModel(this)),
  m_levelNames(new QStringListModel(QStringList{"Low", "Moderate", "High", "Critical"},this))
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

void EditAlertsController::setActive(bool active)
{
  if (active == m_active)
    return;

  if (m_identifyLayersWatcher.isValid() && !m_identifyLayersWatcher.isDone() && !m_identifyLayersWatcher.isCanceled())
    m_identifyLayersWatcher.cancel();

  m_active = active;
  emit activeChanged();
}

void EditAlertsController::addWithinDistanceAlert(int levelIndex, int sourceOverlayIndex, double distance, int itemId, int targetOverlayIndex)
{
  if (levelIndex < 0 ||
      sourceOverlayIndex < 0 ||
      distance < 0.0 ||
      itemId < 0 ||
      targetOverlayIndex < 0)
    return;

  if (sourceOverlayIndex == targetOverlayIndex)
    return;

  AlertLevel level = static_cast<AlertLevel>(levelIndex + 1);
  if (level > AlertLevel::Critical)
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

      if (currIndex == sourceOverlayIndex)
        sourceOverlayMgr = new FeatureOverlayManager(featLayer, this);

      if (currIndex == targetOverlayIndex)
        targetOverlayMgr = new FeatureOverlayManager(featLayer, this);
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

      if (overlay->overlayId().isEmpty())
        continue;

      ++currIndex;

      if (currIndex == sourceOverlayIndex)
        sourceOverlayMgr = new GraphicsOverlayManager(overlay, this);

      if (currIndex == targetOverlayIndex)
        targetOverlayMgr = new GraphicsOverlayManager(overlay, this);
    }
  }

  if (!targetOverlayMgr && !sourceOverlayMgr)
    return;

  GeoElement* targetElement = targetOverlayMgr->elementAt(itemId);
  if (!targetElement)
    return;

  auto createProximityAlert = [this, targetElement, distance, level, sourceOverlayMgr, targetOverlayMgr](int newElement)
  {
    GeoElement* sourceElement = sourceOverlayMgr->elementAt(newElement);
    if (!sourceElement)
      return;

    WithinDistanceAlertConditionData* geofenceAlert = new WithinDistanceAlertConditionData(sourceElement,
                                                               targetElement,
                                                               sourceOverlayMgr,
                                                               targetOverlayMgr,
                                                               distance,
                                                               this);
    geofenceAlert->alertLevel(level);
    geofenceAlert->setName("Within distance!");
    geofenceAlert->setViewed(false);
    AlertListModel::instance()->addAlert(geofenceAlert);
  };

  const int totalElements = static_cast<int>(sourceOverlayMgr->numberOfElements());
  for (qint64 i = 0; i < totalElements; ++i)
    createProximityAlert(i);

  connect(sourceOverlayMgr, &AbstractOverlayManager::elementAdded, this, createProximityAlert);
}

void EditAlertsController::addIntersectsAlert(int levelIndex, int sourceOverlayIndex, int itemId, int targetOverlayIndex)
{
  if (levelIndex < 0 ||
      sourceOverlayIndex < 0 ||
      itemId < 0 ||
      targetOverlayIndex < 0)
    return;

  if (sourceOverlayIndex == targetOverlayIndex)
    return;

  AlertLevel level = static_cast<AlertLevel>(levelIndex + 1);
  if (level > AlertLevel::Critical)
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

      if (currIndex == sourceOverlayIndex)
        sourceOverlayMgr = new FeatureOverlayManager(featLayer, this);

      if (currIndex == targetOverlayIndex)
        targetOverlayMgr = new FeatureOverlayManager(featLayer, this);
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

      if (overlay->overlayId().isEmpty())
        continue;

      ++currIndex;

      if (currIndex == sourceOverlayIndex)
        sourceOverlayMgr = new GraphicsOverlayManager(overlay, this);

      if (currIndex == targetOverlayIndex)
        targetOverlayMgr = new GraphicsOverlayManager(overlay, this);
    }
  }

  if (!targetOverlayMgr && !sourceOverlayMgr)
    return;

  GeoElement* targetElement = targetOverlayMgr->elementAt(itemId);
  if (!targetElement)
    return;

  auto createIntersectsAlert = [this, targetElement, level, sourceOverlayMgr, targetOverlayMgr](int newElement)
  {
    GeoElement* sourceElement = sourceOverlayMgr->elementAt(newElement);
    if (!sourceElement)
      return;

    IntersectsPairAlert* intersectsAlert = new IntersectsPairAlert(sourceElement,
                                                                   targetElement,
                                                                   sourceOverlayMgr,
                                                                   targetOverlayMgr,
                                                                   this);
    intersectsAlert->alertLevel(level);
    intersectsAlert->setName("Intersects!");
    intersectsAlert->setViewed(false);
    AlertListModel::instance()->addAlert(intersectsAlert);
  };

  const int totalElements = static_cast<int>(sourceOverlayMgr->numberOfElements());
  for (qint64 i = 0; i < totalElements; ++i)
    createIntersectsAlert(i);

  connect(sourceOverlayMgr, &AbstractOverlayManager::elementAdded, this, createIntersectsAlert);
}

void EditAlertsController::removeConditionAt(int rowIndex)
{
  AlertListModel::instance()->removeAt(rowIndex);
}

void EditAlertsController::togglePickMode()
{
  m_pickMode = !m_pickMode;

  if (m_pickMode)
  {
    m_mouseClickConnection = connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked,
                                     this, &EditAlertsController::onMouseClicked);

    m_identifyLayersConnection =  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyLayersCompleted,
                                          this, &EditAlertsController::onIdentifyLayersCompleted);

    m_identifyGraphicsConnection =  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyGraphicsOverlaysCompleted,
                                          this, &EditAlertsController::onIdentifyGraphicsOverlaysCompleted);
  }
  else
  {
    disconnect(m_mouseClickConnection);
    disconnect(m_identifyLayersConnection);
    disconnect(m_identifyGraphicsConnection);
  }

  emit pickModeChanged();
}

QAbstractItemModel* EditAlertsController::layerNames() const
{
  return m_layerNames;
}

QAbstractItemModel* EditAlertsController::levelNames() const
{
  return m_levelNames;
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

      if (overlay->overlayId().isEmpty())
        continue;

      newList.append(overlay->overlayId());
    }
  }

  setLayerNames(newList);
}

void EditAlertsController::onMouseClicked(QMouseEvent &event)
{
  if (!isActive())
    return;

  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  if (!m_pickMode)
    return;

  if (m_identifyLayersWatcher.isValid() && !m_identifyLayersWatcher.isDone())
    return;

  if (m_identifyGraphicsWatcher.isValid() && !m_identifyGraphicsWatcher.isDone())
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  m_identifyLayersWatcher = geoView->identifyLayers(event.pos().x(), event.pos().y(), m_tolerance, false);
  m_identifyGraphicsWatcher = geoView->identifyGraphicsOverlays(event.pos().x(), event.pos().y(), m_tolerance, false);

  event.accept();
}

void EditAlertsController::onIdentifyLayersCompleted(const QUuid& taskId, QList<IdentifyLayerResult*> identifyResults)
{
  if (taskId != m_identifyLayersWatcher.taskId())
    return;

  ResultsManager resultsManager(identifyResults);

  if (!isActive())
    return;

  m_identifyLayersWatcher = TaskWatcher();

  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyLayerResult* res = *it;
    if (!res)
      continue;

    const QString layerName = res->layerContent()->name();

    const QList<GeoElement*> geoElements = res->geoElements();
    auto geoElemIt = geoElements.begin();
    auto geoElemEnd = geoElements.end();
    for (; geoElemIt != geoElemEnd; ++geoElemIt)
    {
      GeoElement* geoElement = *geoElemIt;
      if (!geoElement)
        continue;

      AttributeListModel* atts = geoElement->attributes();
      if (!atts)
        return;

      if (atts->containsAttribute("FID"))
      {
        m_identifyGraphicsWatcher.cancel();
        m_identifyGraphicsWatcher = TaskWatcher();
        emit pickedElement(layerName, atts->attributeValue("FID").toInt());
      }
      else if(atts->containsAttribute("OID"))
      {
        m_identifyGraphicsWatcher.cancel();
        m_identifyGraphicsWatcher = TaskWatcher();
        emit pickedElement(layerName, atts->attributeValue("OID").toInt());
      }

      break;
    }
  }
}

void EditAlertsController::onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, QList<IdentifyGraphicsOverlayResult*> identifyResults)
{
  if (taskId != m_identifyGraphicsWatcher.taskId())
    return;

  GraphicsResultsManager resultsManager(identifyResults);

  if (!isActive())
    return;

  m_identifyGraphicsWatcher = TaskWatcher();

  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyGraphicsOverlayResult* res = *it;
    if (!res)
      continue;

    const QString overlayName = res->graphicsOverlay()->overlayId();

    const QList<Graphic*> graphics = res->graphics();
    auto graphicsIt = graphics.begin();
    auto graphicsEnd = graphics.end();
    for (; graphicsIt != graphicsEnd; ++graphicsIt)
    {
      Graphic* graphic = *graphicsIt;
      if (!graphic || !graphic->graphicsOverlay() || !graphic->graphicsOverlay()->graphics())
        continue;

      const int index = graphic->graphicsOverlay()->graphics()->indexOf(graphic);

      m_identifyLayersWatcher.cancel();
      m_identifyLayersWatcher = TaskWatcher();
      emit pickedElement(overlayName, index);

      break;
    }
  }
}

void EditAlertsController::setLayerNames(const QStringList& layerNames)
{
  const QStringList existingNames = m_layerNames->stringList();
  if (existingNames == layerNames)
    return;

  m_layerNames->setStringList(layerNames);
  emit layerNamesChanged();
}

