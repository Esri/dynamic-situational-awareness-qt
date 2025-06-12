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

#include "AlertConditionsController.h"

// C++ API headers
#include "ArcGISFeatureTable.h"
#include "AttributeListModel.h"
#include "DynamicEntity.h"
#include "DynamicEntityObservation.h"
#include "Error.h"
#include "Feature.h"
#include "FeatureIterator.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "Field.h"
#include "GeoView.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "IdentifyLayerResult.h"
#include "Layer.h"
#include "LayerListModel.h"
#include "MapTypes.h"
#include "QueryParameters.h"
#include "ServiceTypes.h"

// Qt headers
#include <QEventLoop>
#include <QFuture>
#include <QJsonArray>
#include <QJsonObject>

// DSA headers
#include "AlertConditionData.h"
#include "AlertConditionListModel.h"
#include "AlertConstants.h"
#include "AlertListModel.h"
#include "AppConstants.h"
#include "AttributeEqualsAlertCondition.h"
#include "FeatureLayerAlertTarget.h"
#include "FixedValueAlertTarget.h"
#include "GeoElementAlertTarget.h"
#include "GraphicsOverlayAlertTarget.h"
#include "GraphicsOverlaysResultsManager.h"
#include "IdentifyController.h"
#include "LayerResultsManager.h"
#include "LocationAlertSource.h"
#include "LocationAlertTarget.h"
#include "MessageFeedConstants.h"
#include "MessagesOverlay.h"
#include "MessagesOverlayAlertTarget.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "WithinAreaAlertCondition.h"
#include "WithinDistanceAlertCondition.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::AlertConditionsController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for working with the conditions which can trigger alerts.

  Alerts are created when a given \l AlertCondition is met.

  This tool presents the list of currently active conditions and allows these to
  be deleted etc.

  This tool allows new \l AlertConditions of various types to be created.

  \sa AlertConditionListModel
  \sa WithinAreaAlertCondition
  \sa WithinDistanceAlertCondition
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
AlertConditionsController::AlertConditionsController(QObject* parent /* = nullptr */):
  AbstractTool(parent),
  m_conditions(new AlertConditionListModel(this)),
  m_sourceNames(new QStringListModel(QStringList{AlertConstants::MY_LOCATION}, this)),
  m_targetNames(new QStringListModel(this)),
  m_levelNames(new QStringListModel(QStringList{"Low priority", "Moderate priority", "High priority", "Critical priority"},this)),
  m_locationSource(new LocationAlertSource(this)),
  m_locationTarget(new LocationAlertTarget(this))
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged,
          this, &AlertConditionsController::onGeoviewChanged);
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::sceneChanged,
        this, &AlertConditionsController::onGeoviewChanged);

  connect(m_conditions, &AlertConditionListModel::rowsInserted, this, &AlertConditionsController::onConditionsChanged);
  connect(m_conditions, &AlertConditionListModel::rowsRemoved, this, &AlertConditionsController::onConditionsChanged);
  connect(m_conditions, &AlertConditionListModel::modelReset, this, &AlertConditionsController::onConditionsChanged);
  connect(m_conditions, &AlertConditionListModel::dataChanged, this, &AlertConditionsController::onConditionsChanged);

  onGeoviewChanged();

  ToolManager::instance().addTool(this);
}

/*!
  \brief Destructor.
 */
AlertConditionsController::~AlertConditionsController()
{
}

/*!
  \brief The name of this tool.
 */
QString AlertConditionsController::toolName() const
{
  return "Alert Conditions";
}

/*! \brief Sets any values in \a properties which are relevant for the alert conditions controller.
 *
 * This tool will use the following key/value pairs in the \a properties map if they are set:
 *
 * \list
 *  \li Conditions. A list of JSON objects describing alert conditions to be added to the map.
 *  \li MessageFeeds. A list of real-time feeds to be used as condition sources.
 * \endlist
 */
void AlertConditionsController::setProperties(const QVariantMap& properties)
{
  const auto conditionsData = properties[AlertConstants::ALERT_CONDITIONS_PROPERTYNAME];

  const auto messageFeeds = properties[MessageFeedConstants::MESSAGE_FEEDS_PROPERTYNAME].toList();
  if (!messageFeeds.isEmpty())
  {
    const auto messageFeedsJson = QJsonArray::fromVariantList(messageFeeds);
    for (const auto& messageFeed : messageFeedsJson)
    {
      const auto messageFeedJsonObject = messageFeed.toObject();
      if (!messageFeedJsonObject.contains(MessageFeedConstants::MESSAGE_FEEDS_NAME) ||
          !messageFeedJsonObject.contains(MessageFeedConstants::MESSAGE_FEEDS_TYPE))
        continue;

      const auto feedName = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_NAME].toString();
      const auto feedType = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_TYPE].toString();

      m_messageFeedTypesToNames.insert(feedType, feedName);
    }

    onLayersChanged();
  }

  if (conditionsData.isNull())
    return;

  const auto conditionsList = conditionsData.toList();
  if (conditionsList.isEmpty())
    return;

  QJsonArray conditionsJsonArray = QJsonArray::fromVariantList(conditionsList);
  if (conditionsJsonArray.isEmpty())
    return;

  // only update stored connections at startup
  if (!m_storedConditions.isEmpty() || m_conditions->rowCount() > 0)
    return;

  auto it = conditionsJsonArray.constBegin();
  auto itEnd = conditionsJsonArray.constEnd();
  for (; it != itEnd; ++it)
  {
    const QJsonValue jsonVal = *it;
    if (jsonVal.isNull())
      continue;

    const QJsonObject jsonObject = jsonVal.toObject();
    if (jsonObject.isEmpty())
      continue;

    m_storedConditions.append(jsonObject);
  };

  addStoredConditions();
}

/*!
  \brief Sets the active state of this tool to \a active.

  \note When active this tool may handle clicks in the view for picking features or graphics.
 */
void AlertConditionsController::setActive(bool active)
{
  if (active == m_active)
    return;

  m_active = active;
  emit activeChanged();

  // if the tool is currently in pick mode and becoming inactive, turn picking off
  if (!m_active && pickMode())
    togglePickMode();
}

/*!
  \brief Adds a \l WithinDistanceAlertCondition to the list of conditions.

  \list
    \li \a conditionName. The name for the condition.
    \li \a levelIndex. The \l AlertLevel for the condition.
    \li \a sourceFeedName. The name of the source feed (e.g. "My Location" or the
      name of a \l Esri::ArcGISRuntime::GraphicsOverlay) used to create an \l AlertSource.
    \li \a distance. The threshold distance for this cobdition in meters.
    \li \a itemId. The item id for the target. If \c -1, then all items in the target will be used.
      In the case where the target is a \l Esri::ArcGISRuntime::GraphicsOverlay, the  id should be the
      index of the graphic. In the case where the target is a \l Esri::ArcGISRuntime::FeatureLayer, the
      id should be the primary key value (e.g. the OID).
    \li \a targetOverlayIndex. The index of the target for the condition in the \l targetNames list. A target can be either
    a \l Esri::ArcGISRuntime::GraphicsOverlay or a \l Esri::ArcGISRuntime::FeatureLayer.
  \endlist

  Returns \c true if the condition was successfully added.
 */
QFuture<bool> AlertConditionsController::addWithinDistanceAlert(const QString& conditionName,
                                                                int levelIndex,
                                                                const QString& sourceFeedName,
                                                                double distance,
                                                                int itemId,
                                                                const QString& targetOverlayName)
{
  const auto future_false = QtFuture::makeReadyValueFuture(false);
  if (levelIndex < 0 ||
      sourceFeedName.isEmpty() ||
      distance < 0.0 ||
      targetOverlayName.isEmpty())
  {
    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid inputs"));
    return future_false;
  }

  AlertLevel level = static_cast<AlertLevel>(levelIndex);
  if (level > AlertLevel::Critical)
  {
    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid Alert Level"));
    return future_false;
  }

  QString targetDescription;
  return targetFromItemIdAndOverlayName(itemId, targetOverlayName, targetDescription).then(this, [=](AlertTarget* target) -> QFuture<bool>
  {
    if (!target)
    {
      emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid Target"));
      return future_false;
    }

    if (sourceFeedName == AlertConstants::MY_LOCATION)
      return addWithinDistanceAlertBySourceLayerType(conditionName, level, sourceFeedName, distance, target, targetDescription, m_locationSource);
    else if (GraphicsOverlay* graphicsOverlay = graphicsOverlayFromName(sourceFeedName); graphicsOverlay)
      return addWithinDistanceAlertBySourceLayerType(conditionName, level, sourceFeedName, distance, target, targetDescription, graphicsOverlay);
    else if (MessagesOverlay* messagesOverlay = messagesOverlayFromName(sourceFeedName); messagesOverlay)
      return addWithinDistanceAlertBySourceLayerType(conditionName, level, sourceFeedName, distance, target, targetDescription, messagesOverlay);

    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QString("Could not find source feed: %1").arg(sourceFeedName));
    return future_false;
  }).result();
}

template<typename T>
QFuture<bool> AlertConditionsController::addWithinDistanceAlertBySourceLayerType(const QString& conditionName,
                                                                                 AlertLevel level,
                                                                                 const QString& sourceFeedName,
                                                                                 double distance,
                                                                                 AlertTarget* target,
                                                                                 const QString& targetDescription,
                                                                                 T* alertSourceLayer)
{
  // make sure the condition has not already been added
  if (conditionAlreadyAdded(conditionName))
    return QtFuture::makeReadyValueFuture(false);

  auto* condition = new WithinDistanceAlertCondition(level, conditionName, distance, this);
  connect(condition, &WithinDistanceAlertCondition::newConditionData, this, &AlertConditionsController::handleNewAlertConditionData);
  condition->init(alertSourceLayer, sourceFeedName, target, targetDescription);
  return QtFuture::makeReadyValueFuture(m_conditions->addAlertCondition(condition));
}

/*!
  \brief Adds a \l WithinAreaAlertCondition to the list of conditions.

  \list
    \li \a conditionName. The name for the condition.
    \li \a levelIndex. The \l AlertLevel for the condition.
    \li \a sourceFeedName. The name of the source feed (e.g. "My Location" or the
      name of a \l Esri::ArcGISRuntime::GraphicsOverlay) used to create an \l AlertSource.
    \li \a itemId. The item id for the target. If \c -1, then all items in the target will be used.
      In the case where the target is a \l Esri::ArcGISRuntime::GraphicsOverlay, the  id should be the
      index of the graphic. In the case where the target is a \l Esri::ArcGISRuntime::FeatureLayer, the
      id should be the primary key value (e.g. the OID).
    \li \a targetOverlayIndex. The index of the target for the condition in the \l targetNames list. A target can be either
    a \l Esri::ArcGISRuntime::GraphicsOverlay or a \l Esri::ArcGISRuntime::FeatureLayer.
  \endlist

  Returns \c true if the condition was successfully added.
 */
QFuture<bool> AlertConditionsController::addWithinAreaAlert(const QString& conditionName,
                                                            int levelIndex,
                                                            const QString& sourceFeedName,
                                                            int itemId,
                                                            const QString& targetOverlayName)
{
  const auto future_false = QtFuture::makeReadyValueFuture(false);
  if (levelIndex < 0 ||
      sourceFeedName.isEmpty() ||
      targetOverlayName.isEmpty())
  {
    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid inputs"));
    return future_false;
  }

  AlertLevel level = static_cast<AlertLevel>(levelIndex);
  if (level > AlertLevel::Critical)
  {
    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid Alert Level"));
    return future_false;
  }

  QString targetDescription;
  return targetFromItemIdAndOverlayName(itemId, targetOverlayName, targetDescription).then(this, [=](AlertTarget* target) -> QFuture<bool>
  {
    if (!target)
    {
      emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid Target"));
      return future_false;
    }

    if (sourceFeedName == AlertConstants::MY_LOCATION)
      return addWithinAreaAlertBySourceLayerType(conditionName, level, sourceFeedName, target, targetDescription, m_locationSource);
    else if (GraphicsOverlay* graphicsOverlay = graphicsOverlayFromName(sourceFeedName); graphicsOverlay)
      return addWithinAreaAlertBySourceLayerType(conditionName, level, sourceFeedName, target, targetDescription, graphicsOverlay);
    else if (MessagesOverlay* messagesOverlay = messagesOverlayFromName(sourceFeedName); messagesOverlay)
      return addWithinAreaAlertBySourceLayerType(conditionName, level, sourceFeedName, target, targetDescription, messagesOverlay);

    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QString("Could not find source feed: %1").arg(sourceFeedName));
    return future_false;
  }).unwrap();
}

template<typename T>
QFuture<bool> AlertConditionsController::addWithinAreaAlertBySourceLayerType(const QString& conditionName,
                                                                             AlertLevel level,
                                                                             const QString& sourceFeedName,
                                                                             AlertTarget* target,
                                                                             const QString& targetDescription,
                                                                             T* alertSourceLayer)
{
  // make sure the condition has not already been added
  if (conditionAlreadyAdded(conditionName))
    return QtFuture::makeReadyValueFuture(false);

  auto* condition = new WithinAreaAlertCondition(level, conditionName, this);
  connect(condition, &WithinAreaAlertCondition::newConditionData, this, &AlertConditionsController::handleNewAlertConditionData);
  condition->init(alertSourceLayer, sourceFeedName, target, targetDescription);
  return QtFuture::makeReadyValueFuture(m_conditions->addAlertCondition(condition));
}

/*!
  \brief Adds a \l AttributeEqualsAlertCondition to the list of conditions.

  \list
    \li \a conditionName. The name for the condition.
    \li \a levelIndex. The \l AlertLevel for the condition.
    \li \a sourceFeedName. The name of the source feed
      (e.g. a \l Esri::ArcGISRuntime::GraphicsOverlay) used to create an \l AlertSource.
    \li \a attributeName. The name of the attribute to query.
    \li \a targetValue. The attribute value to check for.
  \endlist

  Returns \c true if the condition was succesfully added
 */
QFuture<bool> AlertConditionsController::addAttributeEqualsAlert(const QString& conditionName,
                                                                 int levelIndex,
                                                                 const QString& sourceFeedName,
                                                                 const QString& attributeName,
                                                                 const QVariant& targetValue)
{
  const auto future_false = QtFuture::makeReadyValueFuture(false);
  if (levelIndex < 0 ||
      sourceFeedName.isEmpty() ||
      attributeName.isEmpty() ||
      targetValue.isNull())
  {
    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid inputs"));
    return future_false;
  }

  AlertLevel level = static_cast<AlertLevel>(levelIndex);
  if (level > AlertLevel::Critical)
  {
    emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QStringLiteral("Invalid Alert Level"));
    return future_false;
  }

  // pass the source layer that was found by name in either the graphics layers or dynamics layers in operationalLayers
  if (GraphicsOverlay* graphicsOverlay = graphicsOverlayFromName(sourceFeedName); graphicsOverlay)
    return addAttributeEqualsAlertBySourceLayerType(conditionName, level, sourceFeedName, attributeName, targetValue, graphicsOverlay);
  else if (MessagesOverlay* messagesOverlay = messagesOverlayFromName(sourceFeedName); messagesOverlay)
    return addAttributeEqualsAlertBySourceLayerType(conditionName, level, sourceFeedName, attributeName, targetValue, messagesOverlay);

  emit toolErrorOccurred(QStringLiteral("Failed to create Condition"), QString("Could not find source feed: %1").arg(sourceFeedName));
  return future_false;
}

template<typename T>
QFuture<bool> AlertConditionsController::addAttributeEqualsAlertBySourceLayerType(const QString& conditionName,
                                                                                  AlertLevel level,
                                                                                  const QString& sourceFeedName,
                                                                                  const QString& attributeName,
                                                                                  const QVariant& targetValue,
                                                                                  T* alertSourceLayer)
{
  // make sure the condition has not already been added
  if (conditionAlreadyAdded(conditionName))
    return QtFuture::makeReadyValueFuture(false);

  AlertTarget* target = new FixedValueAlertTarget(targetValue, this);
  AttributeEqualsAlertCondition* condition = new AttributeEqualsAlertCondition(level, conditionName, attributeName, this);
  connect(condition, &AttributeEqualsAlertCondition::newConditionData, this, &AlertConditionsController::handleNewAlertConditionData);
  condition->init(alertSourceLayer, sourceFeedName, target, targetValue.toString());
  return QtFuture::makeReadyValueFuture(m_conditions->addAlertCondition(condition));
}

/*!
  \brief Removes the condition at \a rowIndex from the list.
 */
void AlertConditionsController::removeConditionAt(int rowIndex)
{
  AlertCondition* condition = m_conditions->conditionAt(rowIndex);
  if (!condition)
    return;

  m_conditions->removeAt(rowIndex);
  delete condition;
}

/*!
  \brief Toggle the pick mode of the tool.

  When \c true, the tool will attempt to pick a graphic or feature if the user clicks
  in the view.

  \sa pickMode
 */
void AlertConditionsController::togglePickMode()
{
  m_pickMode = !m_pickMode;

  if (m_pickMode)
  {
    m_mouseClickConnection = connect(ToolResourceProvider::instance(), &ToolResourceProvider::mouseClicked,
                                     this, &AlertConditionsController::onMouseClicked);
  }
  else
  {
    disconnect(m_mouseClickConnection);
  }

  emit pickModeChanged();
}

/*
 \brief Updates the name of the given \a rowIndex with \a conditionName.
*/
void AlertConditionsController::updateConditionName(int rowIndex, const QString& conditionName)
{
  auto alertCondition = m_conditions->conditionAt(rowIndex);
  if (!alertCondition)
    return;

  alertCondition->setName(conditionName);
}

/*
 \brief Updates the level of the given \a rowIndex with \a level.
*/
void AlertConditionsController::updateConditionLevel(int rowIndex, int level)
{
  auto alertCondition = m_conditions->conditionAt(rowIndex);
  if (!alertCondition)
    return;

  AlertLevel alertLevel = static_cast<AlertLevel>(level);
  alertCondition->setLevel(alertLevel);
}

/*!
  \property AlertConditionsController::sourceNames
  \brief Returns a QAbstractItemModel containing the list of
  names for creating condition sources.

  \sa AlertSource
 */
QAbstractItemModel* AlertConditionsController::sourceNames() const
{
  return m_sourceNames;
}

/*!
  \property AlertConditionsController::targetNames
  \brief Returns a QAbstractItemModel containing the list of
  names for creating condition targets.

  \sa AlertTarget
 */
QAbstractItemModel* AlertConditionsController::targetNames() const
{
  return m_targetNames;
}

/*!
  \property AlertConditionsController::levelNames
  \brief Returns a QAbstractItemModel containing the list of
  level names for creating condition targets.

  \sa AlertLevel
 */
QAbstractItemModel* AlertConditionsController::levelNames() const
{
  return m_levelNames;
}

/*!
  \property AlertConditionsController::conditionsList
  \brief Returns a QAbstractItemModel containing the list of
  conditions.

  \sa AlertCondition
 */
QAbstractItemModel* AlertConditionsController::conditionsList() const
{
  return m_conditions;
}

/*!
  \property AlertConditionsController::pickMode
  \brief Returns whether the tool is in pick mode or not.
 */
bool AlertConditionsController::pickMode() const
{
  return m_pickMode;
}

/*!
  \brief internal

  Handle changes to the geoView.
 */
void AlertConditionsController::onGeoviewChanged()
{
  setTargetNames(QStringList());
  setSourceNames(QStringList(AlertConstants::MY_LOCATION));

  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  LayerListModel* operationalLayers = ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
  {
    connect(operationalLayers, &LayerListModel::itemAdded, this, &AlertConditionsController::onLayersChanged);
    connect(operationalLayers, &LayerListModel::itemRemoved, this, &AlertConditionsController::onLayersChanged);
  }

  GraphicsOverlayListModel* graphicsOverlays = geoView->graphicsOverlays();
  if (graphicsOverlays)
  {
    connect(graphicsOverlays, &GraphicsOverlayListModel::itemAdded, this, &AlertConditionsController::onLayersChanged);
    connect(graphicsOverlays, &GraphicsOverlayListModel::itemRemoved, this, &AlertConditionsController::onLayersChanged);
  }

  onLayersChanged();
}

/*!
  \brief internal

  Handle changes to the list of layers and graphics overlays in the geoView.

  These data types are the underlying data for \l AlertSource and \l AlertTarget.
 */
void AlertConditionsController::onLayersChanged()
{
  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
  {
    setTargetNames(QStringList());
    setSourceNames(QStringList(AlertConstants::MY_LOCATION));
    return;
  }

  QStringList newTargetList;
  QStringList newSourceList{AlertConstants::MY_LOCATION};
  QStringList existingLayerIds;
  LayerListModel* operationalLayers = ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
  {
    const int opLayersCount = operationalLayers->rowCount();
    for (int i = 0; i < opLayersCount; ++i)
    {
      Layer* lyr = operationalLayers->at(i);
      if (!lyr)
        continue;

      if (FeatureLayer* featureLayer = qobject_cast<FeatureLayer*>(lyr); featureLayer)
      {
        if (featureLayer->loadStatus() != LoadStatus::Loaded)
        {
          connect(featureLayer, &FeatureLayer::doneLoading, this, &AlertConditionsController::onLayersChanged);
        }
        else
        {
          const auto featureLayerName = featureLayer->name();
          newTargetList.append(featureLayerName);
          existingLayerIds.append(featureLayerName);
        }
      }
      else if (MessagesOverlay* messagesOverlay = qobject_cast<MessagesOverlay*>(lyr); messagesOverlay)
      {
        if (messagesOverlay->loadStatus() != LoadStatus::Loaded)
        {
          connect(messagesOverlay, &DynamicEntityLayer::doneLoading, this, &AlertConditionsController::onLayersChanged);
        }
        else
        {
          const auto messagesOverlayLayerName = messagesOverlay->name();
          newTargetList.append(messagesOverlayLayerName);
          newSourceList.append(messagesOverlayLayerName);
          existingLayerIds.append(messagesOverlayLayerName);
        }
      }
    }
  }

  const auto cachedLayerTargetKeys = m_layerTargets.keys();
  for (const QString& key : cachedLayerTargetKeys)
  {
    if (!existingLayerIds.contains(key))
      m_layerTargets.remove(key);
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

      // check whether we have a user facing name defined for the overlay
      const auto overlayIt = m_messageFeedTypesToNames.constFind(overlay->overlayId());
      if (overlayIt != m_messageFeedTypesToNames.constEnd())
      {
        newSourceList.append(overlayIt.value());
        newTargetList.append(overlayIt.value());
      }
      else if (overlay->overlayId() == AppConstants::LAYER_NAME_SCENEVIEW_LOCATION)
      {
        newTargetList.append(AlertConstants::MY_LOCATION);
      }
      else
      {
        newTargetList.append(overlay->overlayId());
      }
    }
  }

  const auto cachedOverlayTargetKeys = m_overlayTargets.keys();
  for (const QString& key : cachedOverlayTargetKeys)
  {
    if (!newSourceList.contains(key))
      m_overlayTargets.remove(key);
  }

  setSourceNames(newSourceList);
  setTargetNames(newTargetList);
  addStoredConditions();
}

const std::optional<qint64> AlertConditionsController::getPickedElementId(const QList<GeoElement*>& geoElements) const
{
  // check again after the completion of the identify and abort if tool
  // is no longer the active tool or there were no geoelements returned
  if (!isActive())
    return std::nullopt;
  if (geoElements.isEmpty())
    return std::nullopt;

  // get the entity id for dynamic entity types
  auto* geoElement = geoElements.first();
  if (auto* observation = dynamic_cast<DynamicEntityObservation*>(geoElement); observation)
  {
    observation->deleteLater();
    return std::make_optional<qint64>(observation->dynamicEntity()->entityId());
  }

  // check the attribute table if feature type
  if (auto* feature = dynamic_cast<Feature*>(geoElement); feature)
  {
    const auto* attributes = feature->attributes();
    auto* table = feature->featureTable();
    if (!attributes)
      return std::nullopt;
    if (!table)
      return std::nullopt;

    const auto primaryKeyField = primaryKeyFieldName(table);
    if (primaryKeyField.isEmpty())
      return std::nullopt;
    if (!attributes->containsAttribute(primaryKeyField))
      return std::nullopt;

    if (auto primaryKeyValue = attributes->attributeValue(primaryKeyField); primaryKeyValue.canConvert<int>())
      return attributes->attributeValue(primaryKeyField).toInt();
    else
      return std::nullopt;
  }

  // graphics, just use the index of the graphic in the layer. not sure we have a case where this
  // might be problematic, but a graphics layer that changes would be a potential issue here.
  if (auto* graphic = dynamic_cast<Graphic*>(geoElement); graphic)
  {
    if (!graphic->graphicsOverlay())
      return std::nullopt;
    if (!graphic->graphicsOverlay()->graphics())
      return std::nullopt;

    return graphic->graphicsOverlay()->graphics()->indexOf(graphic);
  }

  // if the execution makes it to this point, then nothing was found
  return std::nullopt;
}

/*!
  \brief internal

  Handle mouse click events in the view. If active, this will attenpt to pick
  graphics or features.
 */
void AlertConditionsController::onMouseClicked(QMouseEvent &event)
{
  // abort if the tool is not the active tool for some unexplained reason
  if (!isActive())
    return;

  // only respond to left click
  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  // abort if the tool is somehow not in pick mode
  if (!m_pickMode)
    return;

  // get a pointer to the geoView for calling the identify operations
  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  // check for a selected source layer
  if (m_selectedTargetNameIndex > -1)
  {
    // check the graphics overlays for a match of the selected target name
    const QString selectedTargetName = m_targetNames->stringList().at(m_selectedTargetNameIndex);
    auto* graphicsOverlays = geoView->graphicsOverlays();
    for (auto* graphicsOverlay : *graphicsOverlays)
    {
      if (graphicsOverlay->overlayId() == selectedTargetName)
      {
        geoView->identifyGraphicsOverlayAsync(graphicsOverlay, event.position(), m_tolerance,
                                              false, 1, this).then(this, [this, selectedTargetName](IdentifyGraphicsOverlayResult* result)
        {
          const auto resultsManager = std::unique_ptr<IdentifyGraphicsOverlayResult>(result);
          if (const auto pickedElementId = getPickedElementId(resultsManager->geoElements()); pickedElementId.has_value())
          {
            emit pickedElement(selectedTargetName, pickedElementId.value());
            return;
          }
          emit pickedElement(selectedTargetName, -1);
        });
        break;
      }
    }

    // check the dynamic entity layers (messagefeeds) and any feature layers (operationalLayers)
    const auto* operationalLayers = static_cast<SceneView*>(geoView)->arcGISScene()->operationalLayers();
    for (auto* operationalLayer : *operationalLayers)
    {
      if (operationalLayer->name() == selectedTargetName)
      {
        geoView->identifyLayerAsync(operationalLayer, event.position(), m_tolerance,
                                    false, 1, this).then(this, [this, selectedTargetName](IdentifyLayerResult* result)
        {
          const auto resultsManager = std::unique_ptr<IdentifyLayerResult>(result);
          if (const auto pickedElementId = getPickedElementId(resultsManager->geoElements()); pickedElementId.has_value())
          {
            emit pickedElement(selectedTargetName, pickedElementId.value());
            return;
          }
          emit pickedElement(selectedTargetName, -1);
        });
        break;
      }
    }

    togglePickMode();
    event.accept();
    return;
  }

  // dispatch identify calls to the graphics layers and operational layers of the GeoView
  auto identifyLayers = geoView->identifyLayersAsync(event.position(), m_tolerance, false, 1, this);
  auto identifyGraphics = geoView->identifyGraphicsOverlaysAsync(event.position(), m_tolerance, false, 1, this);

  // respond once all QFutures are complete (includes any cancel or failure as well)
  QtFuture::whenAll(identifyLayers, identifyGraphics).then(this, [this](const QList<IdentifyResultsVariant::FutureType> &identifyResults)
  {
    // abort if tool is no longer the active tool
    if (!isActive())
      return;

    // iterate over each type in the results variant
    for (const IdentifyResultsVariant::FutureType& identifyResult : identifyResults)
    {
      if (identifyResult.index() == IdentifyResultsVariant::Types::LAYERS)
      {
        LayerResultsManager resultsManager(std::get<IdentifyResultsVariant::Types::LAYERS>(identifyResult).result());
        for (auto* result : std::as_const(resultsManager.m_results))
        {
          if (!result)
            continue;

          if (const auto pickedElementId = getPickedElementId(result->geoElements()); pickedElementId.has_value())
          {
            emit pickedElement(result->layerContent()->name(), pickedElementId.value());
            return;
          }
        }
      }
      else if (identifyResult.index() == IdentifyResultsVariant::Types::GRAPHICS)
      {
        GraphicsOverlaysResultsManager resultsManager(std::get<IdentifyResultsVariant::Types::GRAPHICS>(identifyResult).result());
        for (auto* result : std::as_const(resultsManager.m_results))
        {
          if (!result)
            continue;

          if (const auto pickedElementId = getPickedElementId(result->geoElements()); pickedElementId.has_value())
          {
            emit pickedElement(result->graphicsOverlay()->overlayId(), pickedElementId.value());
            return;
          }
        }
      }
    }

    // if the execution makes it to this point, then nothing was found
    emit pickedElement(QString{}, -1);
  });

  togglePickMode();
  event.accept();
}

void AlertConditionsController::handleNewAlertConditionData(AlertConditionData* newConditionData)
{
  if (!newConditionData)
    return;
  AlertListModel::instance()->addAlertConditionData(newConditionData);
}

/*!
  \brief internal

  Reports that conditions have changed and emits a JSON representation of all
  active conditions.

  /sa AbstractTool::propertyChanged
 */
void AlertConditionsController::onConditionsChanged()
{
  emit conditionsListChanged();

  QJsonArray allConditionsJson;
  const int conditionsCount = m_conditions->rowCount();
  for(int i = 0; i < conditionsCount; ++i)
  {
    AlertCondition* condition = m_conditions->conditionAt(i);
    if (condition == nullptr)
      continue;

    const QJsonObject conditionJson = conditionToJson(condition);
    if (conditionJson.isEmpty())
      continue;

    allConditionsJson.append(conditionJson);
  }

  for (const QJsonObject& unadded : m_storedConditions)
    allConditionsJson.append(unadded);

  emit propertyChanged(AlertConstants::ALERT_CONDITIONS_PROPERTYNAME, allConditionsJson.toVariantList());
}

/*!
  \brief internal

  Set the list of names for targets to be used when creating conditions.

  \sa AlertCondition
  \sa AlertTarget
 */
void AlertConditionsController::setTargetNames(const QStringList& targetNames)
{
  const QStringList existingNames = m_targetNames->stringList();
  if (existingNames == targetNames)
    return;

  m_targetNames->setStringList(targetNames);
  emit targetNamesChanged();
}

/*!
  \brief internal

  Set the list of names for sources to be used when creating conditions.

  \sa AlertCondition
  \sa AlertSource
 */
void AlertConditionsController::setSourceNames(const QStringList& sourceNames)
{
  const QStringList existingNames = m_sourceNames->stringList();
  if (existingNames == sourceNames)
    return;

  m_sourceNames->setStringList(sourceNames);
  emit sourceNamesChanged();
}

/*!
  \brief internal

  Serialize \a condition to JSON.
 */
QJsonObject AlertConditionsController::conditionToJson(AlertCondition* condition) const
{
  if (!condition)
    return QJsonObject();

  QJsonObject conditionJson;
  conditionJson.insert( AlertConstants::CONDITION_NAME, condition->name());
  conditionJson.insert( AlertConstants::CONDITION_LEVEL, static_cast<int>(condition->level()));
  conditionJson.insert( AlertConstants::CONDITION_TYPE, condition->metaObject()->className());
  conditionJson.insert( AlertConstants::CONDITION_SOURCE, condition->sourceDescription());
  QJsonObject queryObject = QJsonObject::fromVariantMap(condition->queryComponents());
  conditionJson.insert( AlertConstants::CONDITION_QUERY, queryObject);
  conditionJson.insert( AlertConstants::CONDITION_TARGET, condition->targetDescription());

  return conditionJson;
}

/*!
  \brief internal

  Attempt to deserialize \a json to an \l AlertCondition and add it to the app.

  Returns \c true if successful, \c false if not.
 */
QFuture<bool> AlertConditionsController::addConditionFromJson(const QJsonObject& json)
{
  const auto future_false = QtFuture::makeReadyValueFuture(false);
  if (json.isEmpty())
    return future_false;

  auto findString = [json](const QString& key)
  {
    auto findIt = json.constFind(key);
    if (findIt == json.constEnd())
      return QString();

    return findIt.value().toString();
  };

  const QString conditionType = findString(AlertConstants::CONDITION_TYPE);
  if (conditionType.isEmpty())
    return future_false;

  const bool isAttributeEquals = conditionType == AlertConstants::attributeEqualsAlertConditionType();
  const bool isWithinArea = conditionType == AlertConstants::withinAreaAlertConditionType();
  const bool isWithinDistance = conditionType == AlertConstants::withinDistanceAlertConditionType();

  if (!isAttributeEquals && !isWithinArea && !isWithinDistance)
    return future_false;

  auto levelIt = json.constFind(AlertConstants::CONDITION_LEVEL);
  if (levelIt == json.constEnd())
    return future_false;

  const int level = levelIt.value().toInt(0);

  const QString conditionName = findString(AlertConstants::CONDITION_NAME);
  if (conditionName.isEmpty())
    return future_false;

  const QString sourceString = findString(AlertConstants::CONDITION_SOURCE);
  if (sourceString.isEmpty())
    return future_false;

  const QString targetString = findString(AlertConstants::CONDITION_TARGET);
  if (targetString.isEmpty())
    return future_false;

  QJsonObject queryObject = json.value(AlertConstants::CONDITION_QUERY).toObject();
  const QVariantMap queryComponents = queryObject.toVariantMap();

  if (isAttributeEquals)
  {
    const QString attributeName = AttributeEqualsAlertCondition::attributeNameFromQueryComponents(queryComponents);
    if (attributeName.isEmpty())
      return future_false;

    return addAttributeEqualsAlert(conditionName, level, sourceString, attributeName, targetString );
  }
  else if (isWithinArea || isWithinDistance)
  {
    QString targetOverlayName = targetString;
    int itemId = -1;
    if (targetString.contains('[') && targetString.endsWith(']'))
    {
      const int bracketStartIndex = targetString.indexOf('[');
      targetOverlayName = targetString.left(bracketStartIndex).trimmed();
      const int substrLength = (targetString.length()-1) - (bracketStartIndex + 1);
      QString itemString = targetString.mid(bracketStartIndex +1, substrLength);
      bool ok = false;
      itemId = itemString.toInt(&ok);

      if (itemId == -1 || !ok)
        return future_false;
    }

    int targetOverlayIndex = m_targetNames->stringList().indexOf(targetOverlayName);
    if (targetOverlayIndex == -1)
      return future_false;

    QString targetDescription;
    return targetFromItemIdAndOverlayName(itemId, targetOverlayName, targetDescription).then(QtFuture::Launch::Sync, [=](AlertTarget* target) -> QFuture<bool>
    {
      if (!target)
        return future_false;

      if (isWithinArea)
      {
        return addWithinAreaAlert(conditionName, level, sourceString, itemId, targetOverlayName);
      }
      else if (isWithinDistance)
      {
        const double distance = WithinDistanceAlertCondition::getDistanceFromQueryComponents(queryComponents);
        if (distance == -1.0)
          return future_false;

        return addWithinDistanceAlert(conditionName, level, sourceString, distance, itemId, targetOverlayName);
      }
      else
        return future_false;
    }).unwrap();
  }

  return future_false;
}

/*!
  \brief internal

  Attempt to add any stored Conditions serialized as JSON.
 */
void AlertConditionsController::addStoredConditions()
{
  // block signals while we are adding each stored condition
  this->blockSignals(true);
  QList<QFuture<bool>> addStoredConditionFutures;
  for (auto it = m_storedConditions.rbegin(); it != m_storedConditions.rend(); it++)
  {
    // add this future to the list
    addStoredConditionFutures.append(addConditionFromJson(*it));
  }

  // skip the when all if nothing was added
  if (addStoredConditionFutures.count() == 0)
  {
    this->blockSignals(false);
    return;
  }

  // call the completion handler for all adds
  QtFuture::whenAll(addStoredConditionFutures.rbegin(), addStoredConditionFutures.rend()).then(this, [this](const QList<QFuture<bool>> addStoredConditions)
  {
    // store the index of any condition that was successfully added
    int currentConditionId = addStoredConditions.size() - 1;
    for (auto it = addStoredConditions.rbegin(); it != addStoredConditions.rend(); it++)
    {
      if (it->result())
        m_storedConditions.removeAt(currentConditionId);
      currentConditionId--;
    }

    // emit once for all conditions (including stored)
    this->blockSignals(false);
    onConditionsChanged();
  });
}

bool AlertConditionsController::conditionAlreadyAdded(const QString& conditionName)
{
  for (auto i = 0; i < m_conditions->rowCount(); i++)
  {
    const auto* condition = m_conditions->conditionAt(i);
    if (condition->name() == conditionName)
      return true;
  }
  return false;
}

void AlertConditionsController::setSelectedTargetNameIndex(int currentIndex)
{
  m_selectedTargetNameIndex = currentIndex;
}

/*!
  \brief internal
 */
QFuture<AlertTarget*> AlertConditionsController::targetFromItemIdAndOverlayName(int itemId, const QString& targetOverlayName, QString& targetDescription) const
{
  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  AlertTarget* target = nullptr;
  const auto future_null_target = QtFuture::makeReadyValueFuture(target);
  if (!geoView)
    return future_null_target;

  auto* operationalLayers = ToolResourceProvider::instance()->operationalLayers();
  if (operationalLayers)
  {
    const int opLayersCount = operationalLayers->rowCount();
    for (int i = 0; i < opLayersCount; ++i)
    {
      Layer* layer = operationalLayers->at(i);
      if (!layer)
        continue;

      if (layer->name() == targetOverlayName)
      {
        if (FeatureLayer* featureLayer = qobject_cast<FeatureLayer*>(layer); featureLayer)
        {
          if (itemId == -1)
          {
            if (!m_layerTargets.contains(featureLayer->name()))
              m_layerTargets.insert(featureLayer->name(), new FeatureLayerAlertTarget(featureLayer));

            targetDescription = featureLayer->name();
            return QtFuture::makeReadyValueFuture(m_layerTargets.value(featureLayer->name(), nullptr));
          }
          else
          {
            targetDescription = QString("%1 [%2]").arg(featureLayer->name(), QString::number(itemId));
            return targetFromFeatureLayer(featureLayer, itemId);
          }
        }
        else if (MessagesOverlay* messagesOverlay = qobject_cast<MessagesOverlay*>(layer); messagesOverlay)
        {
          if (itemId == -1)
          {
            if (!m_layerTargets.contains(messagesOverlay->name()))
              m_layerTargets.insert(messagesOverlay->name(), new MessagesOverlayAlertTarget(messagesOverlay));

            targetDescription = messagesOverlay->name();
            return QtFuture::makeReadyValueFuture(m_layerTargets.value(messagesOverlay->name(), nullptr));
          }
          else
          {
            targetDescription = QString("%1 [%2]").arg(messagesOverlay->name(), QString::number(itemId));
            return QtFuture::makeReadyValueFuture(targetFromMessagesOverlay(messagesOverlay, itemId));
          }
        }
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

      if (overlay->overlayId() == AppConstants::LAYER_NAME_SCENEVIEW_LOCATION)
      {
        targetDescription = AlertConstants::MY_LOCATION;
        auto* target = static_cast<AlertTarget*>(m_locationTarget);
        return QtFuture::makeReadyValueFuture(target);
      }

      if (overlay->overlayId() != targetOverlayName)
        continue;

      const QString overlayIdOrName = m_messageFeedTypesToNames.value(overlay->overlayId(), overlay->overlayId());

      if (itemId == -1)
      {
        if (!m_overlayTargets.contains(overlayIdOrName))
          m_overlayTargets.insert(overlayIdOrName, new GraphicsOverlayAlertTarget(overlay));

        targetDescription = overlayIdOrName;
        return QtFuture::makeReadyValueFuture(m_overlayTargets.value(overlayIdOrName, nullptr));
      }
      else
      {
        targetDescription = QString("%1 [%2]").arg(overlayIdOrName, QString::number(itemId));
        return QtFuture::makeReadyValueFuture(targetFromGraphicsOverlay(overlay, itemId));
      }
    }
  }

  return future_null_target;
}

/*!
  \brief internal
 */
QFuture<AlertTarget*> AlertConditionsController::targetFromFeatureLayer(FeatureLayer* featureLayer, int itemId) const
{
  AlertTarget* target = nullptr;
  const auto future_null_target = QtFuture::makeReadyValueFuture(target);
  FeatureTable* tab = featureLayer->featureTable();
  if (!tab)
    return future_null_target;

  const QString primaryKey = primaryKeyFieldName(tab);

  QueryParameters qp;
  qp.setWhereClause(QString("\"%1\" = %2").arg(primaryKey, QString::number(itemId)));
  auto* non_const_this = const_cast<AlertConditionsController*>(this);
  return tab->queryFeaturesAsync(qp, non_const_this).then(non_const_this, [future_null_target](FeatureQueryResult* results) -> QFuture<AlertTarget*>
  {
    auto* feature = results->iterator().next();

    if (!feature)
      return future_null_target;

    // capture the new GeoElementAlertTarget here as the parent type
    AlertTarget* target = new GeoElementAlertTarget(feature);
    return QtFuture::makeReadyValueFuture(target);
  }).unwrap();
}

/*!
  \brief internal
 */
AlertTarget* AlertConditionsController::targetFromGraphicsOverlay(GraphicsOverlay* graphicsOverlay, int itemId) const
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

AlertTarget* AlertConditionsController::targetFromMessagesOverlay(MessagesOverlay* messagesOverlay, int itemId) const
{
  if (!messagesOverlay)
    return nullptr;

  auto* e = messagesOverlay->getDynamicEntityById(itemId);
  if (!e)
    return nullptr;

  return new GeoElementAlertTarget(e);
}

/*!
  \brief internal
 */
GraphicsOverlay* AlertConditionsController::graphicsOverlayFromName(const QString& overlayName)
{
  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return nullptr;

  const QString& overlayId = m_messageFeedTypesToNames.key(overlayName, overlayName);

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

      if (overlayId == overlay->overlayId())
        return overlay;
    }
  }

  return nullptr;
}

/*!
  \brief internal
 */
MessagesOverlay* AlertConditionsController::messagesOverlayFromName(const QString& layerName)
{
  auto* operationalLayers = ToolResourceProvider::instance()->operationalLayers();
  if (!operationalLayers)
    return nullptr;

  const QString& layerId = m_messageFeedTypesToNames.key(layerName, layerName);
  for (auto* layer : *operationalLayers)
  {
    if (auto* messagesOverlay = qobject_cast<MessagesOverlay*>(layer); messagesOverlay)
    {
      if (messagesOverlay->layerId() == layerId)
        return messagesOverlay;
    }
  }

  return nullptr;
}

/*!
  \brief internal
 */
QString AlertConditionsController::primaryKeyFieldName(FeatureTable* featureTable) const
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

/*!
  \brief internal
 */
QStringList AlertConditionsController::realtimeFeedTypes() const
{
  return m_messageFeedTypesToNames.keys();
}

QStringList AlertConditionsController::realtimeFeedNames() const
{
  return m_messageFeedTypesToNames.values();
}

} // Dsa

// Signal Documentation
/*!
  \fn void AlertConditionsController::sourceNamesChanged();
  \brief Signal emitted when the source names change.
 */

/*!
  \fn void AlertConditionsController::targetNamesChanged();
  \brief Signal emitted when the target names change.
 */

/*!
  \fn void AlertConditionsController::conditionsListChanged();
  \brief Signal emitted when the conditions list changes.
 */

/*!
  \fn void AlertConditionsController::pickModeChanged();
  \brief Signal emitted when the pick mode changes.
 */

/*!
  \fn void AlertConditionsController::pickedElement(const QString& overlayName, int elementId);
  \brief Signal emitted when an element is picked, passing the selected \a overlayName and \a elementId
  through as parameters.
 */

/*!
  \fn void AlertConditionsController::toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  \brief Signal emitted when an error occurs.

  An \a errorMessage and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */
