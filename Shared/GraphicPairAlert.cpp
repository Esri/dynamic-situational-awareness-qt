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

#include "GraphicPairAlert.h"

#include "Graphic.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

GraphicPairAlert::GraphicPairAlert(Graphic* graphic1, Graphic* graphic2, double distance, QObject* parent):
  AbstractAlert(parent),
  m_graphic1(graphic1),
  m_graphic2(graphic2),
  m_distance(distance)
{

}

GraphicPairAlert::~GraphicPairAlert()
{

}

Geometry GraphicPairAlert::position() const
{
  return m_graphic1 ? m_graphic1->geometry() : Point();
}

void GraphicPairAlert::highlight(bool on)
{
  if (m_graphic1)
    m_graphic1->setSelected(on);
}

Geometry GraphicPairAlert::position2() const
{
  return m_graphic2 ? m_graphic2->geometry() : Point();
}

double GraphicPairAlert::distance() const
{
  return m_distance;
}
