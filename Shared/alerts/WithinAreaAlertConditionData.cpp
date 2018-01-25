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
#include "WithinAreaAlertConditionData.h"

#include "GeoElement.h"
#include "Graphic.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

WithinAreaAlertConditionData::WithinAreaAlertConditionData(Graphic* source,
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

  connect(m_source, &Graphic::destroyed, this, onElementDestroyed);
  connect(m_target, &GeoElement::destroyed, this, onElementDestroyed);

  connect(m_source, &Graphic::geometryChanged, this, &WithinAreaAlertConditionData::positionChanged);
  connect(m_target, &GeoElement::geometryChanged, this, &WithinAreaAlertConditionData::positionChanged);
}

WithinAreaAlertConditionData::~WithinAreaAlertConditionData()
{

}

Geometry WithinAreaAlertConditionData::position() const
{
  return m_source ? m_source->geometry() : Point();
}

void WithinAreaAlertConditionData::highlight(bool on)
{
  m_source->setSelected(on);
}

Geometry WithinAreaAlertConditionData::position2() const
{
  return m_target ? m_target->geometry() : Point();
}
