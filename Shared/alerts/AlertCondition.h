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

#ifndef ALERTCONDITION_H
#define ALERTCONDITION_H

// dsa app headers
#include "AlertLevel.h"

// Qt headers
#include <QList>
#include <QObject>
#include <QVariantMap>

namespace Esri::ArcGISRuntime {
  class GraphicsOverlay;
  class DynamicEntity;
  class DynamicEntityLayer;
}

namespace Dsa {

class AlertConditionData;
class AlertSource;
class AlertTarget;

class AlertCondition : public QObject
{
  Q_OBJECT

public:
  explicit AlertCondition(const AlertLevel& level,
                          const QString& name,
                          QObject* parent = nullptr);

  void init(AlertSource* source, AlertTarget* target, const QString& sourceDescription, const QString& targetDescription);
  void init(Esri::ArcGISRuntime::GraphicsOverlay* sourceFeed, const QString& sourceDescription, AlertTarget* target, const QString& targetDescription);
  void init(Esri::ArcGISRuntime::DynamicEntityLayer* sourceFeed, const QString& sourceDescription, AlertTarget* target, const QString& targetDescription);

  ~AlertCondition();

  AlertLevel level() const;
  void setLevel(AlertLevel level);

  QString name() const;
  void setName(const QString& name);

  QString newConditionDataName() const;

  void addData(AlertConditionData* newData);

  virtual QString queryString() const = 0;
  virtual QVariantMap queryComponents() const = 0;
  virtual AlertConditionData* createData(AlertSource* source, AlertTarget* target) = 0;

  QString sourceDescription() const;
  QString targetDescription() const;
  QString description() const;

  bool isConditionEnabled() const;
  void setConditionEnabled(bool enabled);

signals:
  void noLongerValid();
  void newConditionData(Dsa::AlertConditionData* newConditionData);
  void conditionChanged();
  void conditionEnabledChanged();

private:
  bool m_enabled = true;
  AlertLevel m_level;
  QString m_name;
  QList<AlertConditionData*> m_data;
  QString m_sourceDescription;
  QString m_targetDescription;
};

} // Dsa

#endif // ALERTCONDITIONDATA_H
