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

#ifndef ALERTCONDITIONDATA_H
#define ALERTCONDITIONDATA_H


// example app headers
#include "AlertLevel.h"

// C++ API headers
#include "Point.h"

// Qt headers
#include <QObject>
#include <QString>
#include <QUuid>

namespace Dsa {
namespace Alerts {

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

} // Alerts
} // Dsa

#endif // ALERTCONDITIONDATA_H
