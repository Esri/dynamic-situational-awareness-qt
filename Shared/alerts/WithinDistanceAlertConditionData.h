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

class AlertSpatialTarget;

class WithinDistanceAlertConditionData : public AlertConditionData
{
  Q_OBJECT

public:
  explicit WithinDistanceAlertConditionData(const QString& name,
                                            AlertLevel level,
                                            AlertSource* source,
                                            AlertSpatialTarget* target,
                                            double distance,
                                            QObject* parent = nullptr);
  ~WithinDistanceAlertConditionData();

  double distance() const;
  AlertSpatialTarget* spatialTarget() const;

  bool matchesQuery() const override;

private:
  AlertSpatialTarget* m_spatialTarget;
  double m_distance;
};

#endif // WITHINDISTANCEALERTCONDITIONDATA_H
