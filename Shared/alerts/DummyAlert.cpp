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

#include "DummyAlert.h"

#include "Graphic.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

DummyAlert::DummyAlert(Graphic* graphic, QObject* parent):
  AbstractAlert(parent),
  m_graphic(graphic)
{
  m_graphic->setSelected(true);

  connect(m_graphic, &Graphic::destroyed, this, [this]()
  {
    emit noLongerValid();
  });
}

DummyAlert::~DummyAlert()
{

}

Geometry DummyAlert::position() const
{
  return m_graphic ? m_graphic->geometry() : Point();
}

void DummyAlert::highlight(bool on)
{
  if (m_graphic)
    m_graphic->setSelected(on);
}

QString DummyAlert::description() const
{
  return message();
}

GeoElement* DummyAlert::geoElement() const
{
  return m_graphic;
}
