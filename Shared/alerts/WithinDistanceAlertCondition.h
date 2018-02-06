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

#ifndef WITHINDISTANCEALERTCONDITION_H
#define WITHINDISTANCEALERTCONDITION_H

#include "AlertCondition.h"

#include <QObject>

class WithinDistanceAlertCondition : public AlertCondition
{
  Q_OBJECT

public:
  explicit WithinDistanceAlertCondition(AlertLevel level,
                                        const QString& name,
                                        double distance,
                                        QObject* parent = nullptr);

  ~WithinDistanceAlertCondition();

  AlertConditionData* createData(AlertSource* source, AlertTarget* target) override;

  QString queryString() const override;

  double distance() const;

  static double getDistanceFromQueryString(const QString& queryString);

private:
  double m_distance;
};

#endif // WITHINDISTANCEALERTCONDITION_H
