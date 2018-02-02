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
class AlertSource;
class AlertTarget;

namespace Esri
{
namespace ArcGISRuntime
{
class GraphicsOverlay;
}
}

class AlertCondition : public QObject
{
  Q_OBJECT

public:
  explicit AlertCondition(const AlertLevel& level,
                          const QString& name,
                          QObject* parent = nullptr);

  void init(AlertSource* source, AlertTarget* target);
  void init(Esri::ArcGISRuntime::GraphicsOverlay* sourceFeed, AlertTarget* target);

  ~AlertCondition();

  AlertLevel level() const;
  void setLevel(AlertLevel level);

  QString name() const;
  void setName(const QString& name);

  QString newConditionDataName() const;

  void addData(AlertConditionData* newData);

  virtual AlertConditionData* createData(AlertSource* source, AlertTarget* target) = 0;

signals:
  void noLongerValid();

private:
  AlertLevel m_level;
  QString m_name;
  QList<AlertConditionData*> m_data;
};

#endif // ALERTCONDITIONDATA_H
