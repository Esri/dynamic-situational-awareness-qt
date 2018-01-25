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

#include <QDebug>

using namespace Esri::ArcGISRuntime;

WithinDistanceAlertConditionData::WithinDistanceAlertConditionData(Graphic* source,
                                       GeoElement* target,
                                       double distance,
                                       AlertCondition* condition):
  AlertConditionData(condition),
  m_source(source),
  m_target(target),
  m_distance(distance)
{
  qDebug() << "condition data created";

  auto onElementDestroyed = [this]()
  {
    m_source = nullptr;
    m_target = nullptr;
    emit noLongerValid();
  };

  connect(m_source, &GeoElement::destroyed, this, onElementDestroyed);
  connect(m_target, &GeoElement::destroyed, this, onElementDestroyed);

  connect(m_source, &Graphic::geometryChanged, this, &WithinDistanceAlertConditionData::positionChanged);
  connect(m_target, &GeoElement::geometryChanged, this, &WithinDistanceAlertConditionData::positionChanged);
}

WithinDistanceAlertConditionData::~WithinDistanceAlertConditionData()
{

}

Geometry WithinDistanceAlertConditionData::position() const
{
  return m_source ? m_source->geometry() : Point();
}

void WithinDistanceAlertConditionData::highlight(bool on)
{
  m_source->setSelected(on);
}

Geometry WithinDistanceAlertConditionData::position2() const
{
  return m_target ? m_target->geometry() : Point();
}

double WithinDistanceAlertConditionData::distance() const
{
  return m_distance;
}
