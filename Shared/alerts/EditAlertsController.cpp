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
#include "AlertConditionListModel.h"
#include "EditAlertsController.h"
#include "FeatureLayerAlertTarget.h"
#include "GeoElementAlertTarget.h"
#include "GraphicsOverlayAlertTarget.h"
#include "LocationAlertSource.h"
#include "WithinAreaAlertCondition.h"
#include "WithinDistanceAlertCondition.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "ArcGISFeatureTable.h"
#include "GeoView.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "FeatureLayer.h"
#include "Layer.h"
#include "LayerListModel.h"

#include <QEventLoop>

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
  m_conditions(new AlertConditionListModel(this)),
  m_sourceNames(new QStringListModel(QStringList{"My Location"}, this)),
  m_targetNames(new QStringListModel(this)),
  m_levelNames(new QStringListModel(QStringList{"Low priority", "Moderate priority", "High priority", "Critical priority"},this)),
  m_locationSource(new LocationAlertSource(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged,
          this, &EditAlertsController::onGeoviewChanged);

  connect(m_conditions, &AlertConditionListModel::dataChanged, this, &EditAlertsController::conditionsListChanged);

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

void EditAlertsController::addWithinDistanceAlert(const QString& conditionName, int levelIndex, const QString& sourceFeedName, double distance, int itemId, int targetOverlayIndex)
{
  if (levelIndex < 0 ||
      sourceFeedName.isEmpty() ||
      distance < 0.0 ||
      targetOverlayIndex < 0)
    return;

  AlertLevel level = static_cast<AlertLevel>(levelIndex + 1);
  if (level > AlertLevel::Critical)
    return;

  AlertTarget* target = targetFromItemIdAndIndex(itemId, targetOverlayIndex);
  if (!target)
    return;

  if (sourceFeedName == "My Location")
  {
    WithinDistanceAlertCondition* condition = new WithinDistanceAlertCondition(level, conditionName, distance, this);
    condition->init(m_locationSource, target);
    m_conditions->addAlertCondition(condition);
  }
  else
  {
    GraphicsOverlay* sourceOverlay = graphicsOverlayFromName(sourceFeedName);
    if (sourceOverlay)
    {
      WithinDistanceAlertCondition* condition = new WithinDistanceAlertCondition(level, conditionName, distance, this);
      condition->init(sourceOverlay, target);
      m_conditions->addAlertCondition(condition);
    }
  }
}

void EditAlertsController::addWithinAreaAlert(const QString& conditionName, int levelIndex, const QString& sourceFeedName, int itemId, int targetOverlayIndex)
{
  if (levelIndex < 0 ||
      sourceFeedName.isEmpty() ||
      targetOverlayIndex < 0)
    return;

  AlertLevel level = static_cast<AlertLevel>(levelIndex + 1);
  if (level > AlertLevel::Critical)
    return;

  AlertTarget* target = targetFromItemIdAndIndex(itemId, targetOverlayIndex);
  if (!target)
    return;

  if (sourceFeedName == "My Location")
  {
    WithinAreaAlertCondition* condition = new WithinAreaAlertCondition(level, conditionName, this);
    condition->init(m_locationSource, target);
    m_conditions->addAlertCondition(condition);
  }
  else
  {
    GraphicsOverlay* sourceOverlay = graphicsOverlayFromName(sourceFeedName);
    if (sourceOverlay)
    {
     WithinAreaAlertCondition* condition = new WithinAreaAlertCondition(level, conditionName, this);
      condition->init(sourceOverlay, target);
      m_conditions->addAlertCondition(condition);
    }
  }
}

void EditAlertsController::removeConditionAt(int rowIndex)
{
  AlertCondition* condition = m_conditions->conditionAt(rowIndex);
  m_conditions->removeAt(rowIndex);
  delete condition;
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

QAbstractItemModel* EditAlertsController::sourceNames() const
{
  return m_sourceNames;
}

QAbstractItemModel* EditAlertsController::targetNames() const
{
  return m_targetNames;
}

QAbstractItemModel* EditAlertsController::levelNames() const
{
  return m_levelNames;
}

QAbstractItemModel* EditAlertsController::conditionsList() const
{
  return m_conditions;
}

bool EditAlertsController::pickMode() const
{
  return m_pickMode;
}

void EditAlertsController::onGeoviewChanged()
{
  setTargetNames(QStringList());
  setSourceNames(QStringList("My Location"));

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
    setTargetNames(QStringList());
    setSourceNames(QStringList("My Location"));
    return;
  }

  QStringList newTargetList;
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
        newTargetList.append(featLayer->name());
    }
  }

  QStringList newSourceList{"My Location"};
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

      if (realtimeFeedNames().contains(overlay->overlayId()))
        newSourceList.append(overlay->overlayId());

      newTargetList.append(overlay->overlayId());
    }
  }

  setSourceNames(newSourceList);
  setTargetNames(newTargetList);
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

      Feature* feature = qobject_cast<Feature*>(geoElement);
      if (!feature)
        continue;

      FeatureTable* table = feature->featureTable();
      if (!table)
        continue;

      QString primaryKeyField = primaryKeyFieldName(table);

      if (primaryKeyField.isEmpty())
        continue;

      if (!atts->containsAttribute(primaryKeyField))
        continue;

      m_identifyGraphicsWatcher.cancel();
      m_identifyGraphicsWatcher = TaskWatcher();
      emit pickedElement(layerName, atts->attributeValue(primaryKeyField).toInt());

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

void EditAlertsController::setTargetNames(const QStringList& targetNames)
{
  const QStringList existingNames = m_targetNames->stringList();
  if (existingNames == targetNames)
    return;

  m_targetNames->setStringList(targetNames);
  emit targetNamesChanged();
}

void EditAlertsController::setSourceNames(const QStringList& sourceNames)
{
  const QStringList existingNames = m_sourceNames->stringList();
  if (existingNames == sourceNames)
    return;

  m_sourceNames->setStringList(sourceNames);
  emit sourceNamesChanged();
}

AlertTarget *EditAlertsController::targetFromItemIdAndIndex(int itemId, int targetOverlayIndex) const
{
  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return nullptr;

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

      if (currIndex == targetOverlayIndex)
      {
        if (itemId == -1)
          return new FeatureLayerAlertTarget(featLayer);
        else
          return targetFromFeatureLayer(featLayer, itemId);
      }
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

      if (currIndex == targetOverlayIndex)
      {
        if (itemId == -1)
          return new GraphicsOverlayAlertTarget(overlay);
        else
          return targetFromGraphicsOverlay(overlay, itemId);
      }
    }
  }

  return nullptr;
}

AlertTarget* EditAlertsController::targetFromFeatureLayer(FeatureLayer* featureLayer, int itemId) const
{
  FeatureTable* tab = featureLayer->featureTable();
  if (!tab)
    return nullptr;

  const QString primaryKey = primaryKeyFieldName(tab);

  QueryParameters qp;
  qp.setWhereClause(QString("\"%1\" = %2").arg(primaryKey, QString::number(itemId)));

  QEventLoop loop;
  tab->queryFeatures(qp);

  connect(tab, &FeatureTable::errorOccurred, this, [this, &loop](Error)
  {
    loop.quit();
  });

  Feature* feature = nullptr;
  auto connection = loop.connect(tab, &FeatureTable::queryFeaturesCompleted, this, [this, &loop, &feature](QUuid, FeatureQueryResult* featureQueryResult)
  {
    loop.quit();

    if (featureQueryResult)
      feature = featureQueryResult->iterator().next();
  });

  loop.exec();

  disconnect(connection);

  if (!feature)
    return nullptr;

  return new GeoElementAlertTarget(feature);
}

AlertTarget* EditAlertsController::targetFromGraphicsOverlay(GraphicsOverlay* graphicsOverlay, int itemId) const
{
  if (!graphicsOverlay)
    return nullptr;

  GraphicListModel* graphics = graphicsOverlay->graphics();
  if (!graphics)
    return nullptr;

  Graphic* g = graphics->at(itemId);
  if (!g)
    return nullptr;

  return new GeoElementAlertTarget(g);
}

GraphicsOverlay* EditAlertsController::graphicsOverlayFromName(const QString& overlayName)
{
  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return nullptr;

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

      if (overlayName == overlay->overlayId())
        return overlay;
    }
  }

  return nullptr;
}

QString EditAlertsController::primaryKeyFieldName(FeatureTable* featureTable) const
{
  if (!featureTable)
    return QString();

  ArcGISFeatureTable* agsFeatureTable = qobject_cast<ArcGISFeatureTable*>(featureTable);
  if (agsFeatureTable)
  {
    return agsFeatureTable->objectIdField();
  }
  else
  {
    const QList<Field> fields = featureTable->fields();
    for( const Field& field : fields)
    {
      if (field.fieldType() == FieldType::OID)
        return field.name();
    }
  }

  return QString();
}

QStringList EditAlertsController::realtimeFeedNames()
{
  return QStringList{"cot"};
}

