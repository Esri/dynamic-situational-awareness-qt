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

#include "AbstractOverlayManager.h"
#include "WithinDistanceAlertConditionData.h"

#include "GeoElement.h"
#include "Graphic.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

WithinDistanceAlertConditionData::WithinDistanceAlertConditionData(AlertCondition* condition,
                                                                   AlertSource* source,
                                                                   GeoElement* target,
                                                                   double distance):
  AlertConditionData(condition, source),
  m_target(target),
  m_distance(distance)
{
  connect(m_target, &GeoElement::destroyed, this, [this]()
  {
    m_target = nullptr;
    emit noLongerValid();
  });

  connect(m_target, &GeoElement::geometryChanged, this, &WithinDistanceAlertConditionData::positionChanged);
}

WithinDistanceAlertConditionData::~WithinDistanceAlertConditionData()
{

}

Geometry WithinDistanceAlertConditionData::position2() const
{
  return m_target ? m_target->geometry() : Point();
}

double WithinDistanceAlertConditionData::distance() const
{
  return m_distance;
}
