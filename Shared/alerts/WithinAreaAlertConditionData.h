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
  explicit WithinAreaAlertConditionData(Esri::ArcGISRuntime::Graphic* source,
                              Esri::ArcGISRuntime::GeoElement* target,
                              AlertCondition* parent = nullptr);
  ~WithinAreaAlertConditionData();

  void highlight(bool on) override;

  Esri::ArcGISRuntime::Geometry position() const override;
  Esri::ArcGISRuntime::Geometry position2() const;

private:
  Esri::ArcGISRuntime::Graphic* m_source;
  Esri::ArcGISRuntime::GeoElement* m_target;
};

#endif // WITHINAREAALERTCONDITIONDATA_H
