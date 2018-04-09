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


// example app headers
#include "AlertConditionData.h"

// C++ API headers
#include "Geometry.h"

namespace Dsa {
namespace Alerts {

class WithinDistanceAlertConditionData : public AlertConditionData
{
  Q_OBJECT

public:
  WithinDistanceAlertConditionData(const QString& name,
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

} // Alerts
} // Dsa

#endif // WITHINDISTANCEALERTCONDITIONDATA_H
