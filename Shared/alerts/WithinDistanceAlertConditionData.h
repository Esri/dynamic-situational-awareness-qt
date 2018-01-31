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
  explicit WithinDistanceAlertConditionData(AlertCondition* condition,
                                            AlertSource* source,
                                            AlertTarget* target,
                                            double distance);
  ~WithinDistanceAlertConditionData();

  double distance() const;

private:
  double m_distance;
};

#endif // WITHINDISTANCEALERTCONDITIONDATA_H
