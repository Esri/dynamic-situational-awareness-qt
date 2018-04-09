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

#ifndef WITHINAREAALERTCONDITIONDATA_H
#define WITHINAREAALERTCONDITIONDATA_H


// example app headers
#include "AlertConditionData.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
class Graphic;
}
}

namespace Dsa {
namespace Alerts {

class WithinAreaAlertConditionData : public AlertConditionData
{
  Q_OBJECT

public:
  WithinAreaAlertConditionData(const QString& name,
                               AlertLevel level,
                               AlertSource* source,
                               AlertTarget* target,
                               QObject* parent = nullptr);

  ~WithinAreaAlertConditionData();

  bool matchesQuery() const override;
};

} // Alerts
} // Dsa

#endif // WITHINAREAALERTCONDITIONDATA_H
