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

#ifndef WITHINDISTANCEALERTCONDITIONDATA_H
#define WITHINDISTANCEALERTCONDITIONDATA_H

#include "AlertConditionData.h"
#include "Geometry.h"

class WithinDistanceAlertConditionData : public AlertConditionData
{
  Q_OBJECT

public:
  explicit WithinDistanceAlertConditionData(const QString& name,
                                            AlertLevel level,
                                            AlertSource* source,
                                            AlertTarget* target,
                                            double distance,
                                            QObject* parent = nullptr);
  ~WithinDistanceAlertConditionData();

  double distance() const;

  bool matchesQuery() const override;

private:
  double m_distance = 0.0;
  double m_moveDistance = 0.0;
};

#endif // WITHINDISTANCEALERTCONDITIONDATA_H
