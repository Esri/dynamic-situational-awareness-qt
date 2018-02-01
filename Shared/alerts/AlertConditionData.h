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

#include "AlertLevel.h"

#include "Point.h"

#include <QObject>
#include <QString>
#include <QUuid>

class AlertSource;
class AlertTarget;

class AlertConditionData : public QObject
{
  Q_OBJECT

public:
  explicit AlertConditionData(const QString& name, AlertLevel level, AlertSource* source, AlertTarget* target, QObject* parent = nullptr);
  ~AlertConditionData();

  AlertLevel level() const;

  Esri::ArcGISRuntime::Point sourceLocation() const;

  void highlight(bool on);

  QString name() const;

  QUuid id() const;
  void setId(const QUuid& id);

  bool viewed() const;
  void setViewed(bool viewed);

  bool active() const;
  void setActive(bool active);

  AlertSource* source() const;
  AlertTarget* target() const;

  virtual bool matchesQuery() const = 0;

signals:
  void statusChanged();
  void viewedChanged();
  void dataChanged();
  void activeChanged();
  void noLongerValid();

private:
  QString m_name;
  AlertLevel m_level = AlertLevel::Unknown;
  AlertSource* m_source = nullptr;
  AlertTarget* m_target = nullptr;
  QUuid m_id;
  bool m_viewed = false;
  bool m_active = false;
};

#endif // ALERTCONDITIONDATA_H
