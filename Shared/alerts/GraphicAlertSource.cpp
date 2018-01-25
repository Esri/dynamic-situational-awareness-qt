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

#include "GraphicAlertSource.h"

#include "Envelope.h"
#include "Graphic.h"

using namespace Esri::ArcGISRuntime;


GraphicAlertSource::GraphicAlertSource(Graphic* graphic):
  AlertSource(graphic),
  m_graphic(graphic)
{
  connect(m_graphic, &Graphic::geometryChanged, this, &GraphicAlertSource::positionChanged);
}

GraphicAlertSource::~GraphicAlertSource()
{

}

Point GraphicAlertSource::position() const
{
  if (m_graphic->geometry().geometryType() == GeometryType::Point)
    return m_graphic->geometry();
  else
    return m_graphic->geometry().extent().center();
}

void GraphicAlertSource::setSelected(bool selected)
{
  m_graphic->setSelected(selected);
}
