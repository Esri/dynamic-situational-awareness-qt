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

#include "AlertConditionData.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
class Graphic;
}
}

class AbstractOverlayManager;

class WithinAreaAlertConditionData : public AlertConditionData
{
  Q_OBJECT

public:
  explicit WithinAreaAlertConditionData(AlertCondition* condition,
                                        AlertSource* source,
                                        Esri::ArcGISRuntime::GeoElement* target);

  ~WithinAreaAlertConditionData();

  Esri::ArcGISRuntime::Geometry targetLocation() const;

private:
  Esri::ArcGISRuntime::GeoElement* m_target;
};

#endif // WITHINAREAALERTCONDITIONDATA_H
