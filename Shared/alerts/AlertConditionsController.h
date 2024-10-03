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

#ifndef ALERTCONDITIONSCONTROLLER_H
#define ALERTCONDITIONSCONTROLLER_H

// Qt headers
#include <QFuture>
#include <QHash>
#include <QJsonObject>
#include <QStringListModel>

// STL headers
#include <optional>

// DSA headers
#include "AbstractTool.h"
#include "AlertLevel.h"

class QMouseEvent;

namespace Esri::ArcGISRuntime {
  class IdentifyLayerResult;
  class IdentifyGraphicsOverlayResult;
  class FeatureLayer;
  class FeatureTable;
  class GraphicsOverlay;
}

Q_MOC_INCLUDE("IdentifyGraphicsOverlayResult.h")
Q_MOC_INCLUDE("IdentifyLayerResult.h")

namespace Dsa {
class AlertCondition;
class AlertConditionData;
class AlertConditionListModel;
class AlertTarget;
class LocationAlertSource;
class LocationAlertTarget;
class MessagesOverlay;

using AlertTargetOptional = std::optional<AlertTarget*>;
using AlertTargetFuture = QFuture<AlertTargetOptional>;

class AlertConditionsController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* sourceNames READ sourceNames NOTIFY sourceNamesChanged)
  Q_PROPERTY(QAbstractItemModel* targetNames READ targetNames NOTIFY targetNamesChanged)
  Q_PROPERTY(QAbstractItemModel* levelNames READ levelNames CONSTANT)
  Q_PROPERTY(QAbstractItemModel* conditionsList READ conditionsList NOTIFY conditionsListChanged)
  Q_PROPERTY(bool pickMode READ pickMode NOTIFY pickModeChanged)

public:

  explicit AlertConditionsController(QObject* parent = nullptr);
  ~AlertConditionsController();

  // AbstractTool interface
  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  void setActive(bool active) override;

  Q_INVOKABLE QFuture<bool> addWithinDistanceAlert(const QString& conditionName, int levelIndex, const QString& sourceFeedname, double distance, int itemId, int targetOverlayIndex);
  Q_INVOKABLE QFuture<bool> addWithinAreaAlert(const QString& conditionName, int levelIndex, const QString& sourceFeedname, int itemId, int targetOverlayIndex);
  Q_INVOKABLE QFuture<bool> addAttributeEqualsAlert(const QString& conditionName, int levelIndex, const QString& sourceFeedname, const QString& attributeName, const QVariant& targetValue);
  Q_INVOKABLE void removeConditionAt(int rowIndex);
  Q_INVOKABLE void togglePickMode();
  Q_INVOKABLE void updateConditionName(int rowIndex, const QString& conditionName);
  Q_INVOKABLE void updateConditionLevel(int rowIndex, int level);

  QAbstractItemModel* sourceNames() const;
  QAbstractItemModel* targetNames() const;
  QAbstractItemModel* levelNames() const;
  QAbstractItemModel* conditionsList() const;
  bool pickMode() const;

signals:
  void sourceNamesChanged();
  void targetNamesChanged();
  void conditionsListChanged();
  void pickModeChanged();
  void pickedElement(const QString& overlayName, int elementId);
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

private slots:
  void onGeoviewChanged();
  void onLayersChanged();
  void onMouseClicked(QMouseEvent& event);
  void handleNewAlertConditionData(AlertConditionData* newConditionData);
  void onConditionsChanged();

private:
  void setTargetNames(const QStringList& targetNames);
  void setSourceNames(const QStringList& sourceNames);
  QJsonObject conditionToJson(AlertCondition* condition) const;
  QFuture<bool> addConditionFromJson(const QJsonObject& json);
  void addStoredConditions();

  AlertTargetFuture targetFromItemIdAndIndex(int itemId, int targetOverlayIndex, QString& targetDescription) const;
  AlertTargetFuture targetFromFeatureLayer(Esri::ArcGISRuntime::FeatureLayer* featureLayer, int itemId) const;
  AlertTarget* targetFromGraphicsOverlay(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay, int itemId) const;
  AlertTarget* targetFromMessagesOverlay(Dsa::MessagesOverlay* messagesOverlay, int itemId) const;
  Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlayFromName(const QString& overlayName);
  MessagesOverlay* messagesOverlayFromName(const QString& layerName);
  QString primaryKeyFieldName(Esri::ArcGISRuntime::FeatureTable* featureTable) const;

  template<typename T>
  QFuture<bool> addWithinDistanceAlertBySourceLayerType(const QString& conditionName, AlertLevel level, const QString& sourceFeedName, double distance, AlertTarget* target, const QString& targetDescription, T* alertSourceLayer);

  template<typename T>
  QFuture<bool> addWithinAreaAlertBySourceLayerType(const QString& conditionName, AlertLevel level, const QString& sourceFeedName, AlertTarget* target, const QString& targetDescription, T* alertSourceLayer);

  template<typename T>
  QFuture<bool> addAttributeEqualsAlertBySourceLayerType(const QString& conditionName, AlertLevel level, const QString& sourceFeedName, const QString& attributeName, const QVariant& targetValue, T* alertTarget);

  QStringList realtimeFeedTypes() const;
  QStringList realtimeFeedNames() const;

  AlertConditionListModel* m_conditions;
  QStringListModel* m_sourceNames;
  QStringListModel* m_targetNames;
  QStringListModel* m_levelNames;
  bool m_pickMode = false;
  double m_tolerance = 5;
  LocationAlertSource* m_locationSource = nullptr;
  LocationAlertTarget* m_locationTarget = nullptr;
  mutable QHash<QString,AlertTarget*> m_layerTargets;
  mutable QHash<QString,AlertTarget*> m_overlayTargets;
  QList<QJsonObject> m_storedConditions;
  QHash<QString,QString> m_messageFeedTypesToNames;

  QMetaObject::Connection m_mouseClickConnection;
};

} // Dsa

#endif // ALERTCONDITIONSCONTROLLER_H
