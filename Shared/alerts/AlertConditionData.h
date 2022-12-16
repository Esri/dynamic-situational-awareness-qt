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

#ifndef ALERTCONDITIONDATA_H
#define ALERTCONDITIONDATA_H

// dsa app headers
#include "AlertLevel.h"

namespace Esri::ArcGISRuntime {
  class Point;
}

// Qt headers
#include <QObject>
#include <QString>
#include <QUuid>

namespace Dsa {

class AlertSource;
class AlertTarget;

class AlertConditionData : public QObject
{
  Q_OBJECT

public:
  AlertConditionData(const QString& name, AlertLevel level, AlertSource* source, AlertTarget* target, QObject* parent = nullptr);
  ~AlertConditionData();

  AlertLevel level() const;
  void setLevel(AlertLevel level);

  Esri::ArcGISRuntime::Point sourceLocation() const;

  void highlight(bool on);

  QString name() const;
  void setName(const QString& name);

  QUuid id() const;
  void setId(const QUuid& id);

  bool viewed() const;
  void setViewed(bool viewed);

  bool isActive() const;

  AlertSource* source() const;
  AlertTarget* target() const;

  virtual bool matchesQuery() const = 0;

  bool cachedQueryResult() const;
  bool isQueryOutOfDate() const;

  bool isConditionEnabled() const;
  void setConditionEnabled(bool isConditionEnabled);

signals:
  void statusChanged();
  void viewedChanged();
  void dataChanged();
  void activeChanged();
  void noLongerValid();

private slots:
  void handleDataChanged();

private:
  void setActive(bool active);

  QString m_name;
  AlertLevel m_level = AlertLevel::Unknown;
  AlertSource* m_source = nullptr;
  AlertTarget* m_target = nullptr;
  QUuid m_id;
  bool m_enabled = true;
  bool m_viewed = false;
  bool m_active = false;
  bool m_queryOutOfDate = true;
  mutable bool m_cachedQueryResult = false;
};

} // Dsa

#endif // ALERTCONDITIONDATA_H
