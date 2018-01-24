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
#include "IntersectsPairAlert.h"

#include "GeoElement.h"
#include "Graphic.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

IntersectsPairAlert::IntersectsPairAlert(Graphic* source,
                                         GeoElement* target,
                                         AlertCondition* condition):
  AlertConditionData(condition),
  m_source(source),
  m_target(target)
{
  auto onElementDestroyed = [this]()
  {
    m_source = nullptr;
    m_target = nullptr;
    emit noLongerValid();
  };

  connect(m_source, &GeoElement::destroyed, this, onElementDestroyed);
  connect(m_target, &GeoElement::destroyed, this, onElementDestroyed);
}

IntersectsPairAlert::~IntersectsPairAlert()
{

}

Geometry IntersectsPairAlert::position() const
{
  return m_source ? m_source->geometry() : Point();
}

void IntersectsPairAlert::highlight(bool on)
{
  m_source->setSelected(on);
}

Geometry IntersectsPairAlert::position2() const
{
  return m_target ? m_target->geometry() : Point();
}

GeoElement* IntersectsPairAlert::geoElement() const
{
  return m_source;
}
