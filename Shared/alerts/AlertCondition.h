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

#ifndef ALERTCONDITION_H
#define ALERTCONDITION_H

#include "AlertLevel.h"

#include <QList>
#include <QObject>

class AlertConditionData;

class AlertCondition : public QObject
{
  Q_OBJECT

public:
  explicit AlertCondition(const AlertLevel& level, const QString& name, QObject* parent = nullptr);
  ~AlertCondition();

  AlertLevel level() const;
  void alertLevel(const AlertLevel& level);

  QString name() const;
  void setName(const QString& name);

  void addData(AlertConditionData* newData);

signals:
  void noLongerValid();

private:
  AlertLevel m_level;
  QString m_name;
};

#endif // ALERTCONDITIONDATA_H
