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

#include "AlertSource.h"
#include "AbstractOverlayManager.h"
#include "WithinAreaAlertConditionData.h"

#include "GeoElement.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

WithinAreaAlertConditionData::WithinAreaAlertConditionData(AlertCondition* condition,
                                                           AlertSource* source,
                                                           GeoElement* target):
  AlertConditionData(condition, source),
  m_target(target)
{
  connect(m_target, &GeoElement::destroyed, this, [this]()
  {
    m_target = nullptr;
    emit noLongerValid();
  });

  connect(m_target, &GeoElement::geometryChanged, this, &WithinAreaAlertConditionData::positionChanged);
}

WithinAreaAlertConditionData::~WithinAreaAlertConditionData()
{

}

Geometry WithinAreaAlertConditionData::position2() const
{
  return m_target ? m_target->geometry() : Point();
}
