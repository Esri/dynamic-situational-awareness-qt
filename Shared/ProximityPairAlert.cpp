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

#include "ProximityPairAlert.h"

#include "GeoElement.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

ProximityPairAlert::ProximityPairAlert(GeoElement* element1, GeoElement* element2, double distance, QObject* parent):
  AbstractAlert(parent),
  m_element1(element1),
  m_element2(element2),
  m_distance(distance)
{
  auto onElementDestroyed = [this]()
  {
    m_element1 = nullptr;
    m_element2 = nullptr;
    emit noLongerValid();
  };

  connect(m_element1, &GeoElement::destroyed, this, onElementDestroyed);
  connect(m_element2, &GeoElement::destroyed, this, onElementDestroyed);
}

ProximityPairAlert::~ProximityPairAlert()
{

}

Geometry ProximityPairAlert::position() const
{
  return m_element1 ? m_element1->geometry() : Point();
}

QString ProximityPairAlert::description() const
{
  return QString(element1Description() + " within %1 m of " + element2Description()).arg(m_distance);
}

Geometry ProximityPairAlert::position2() const
{
  return m_element2 ? m_element2->geometry() : Point();
}

double ProximityPairAlert::distance() const
{
  return m_distance;
}
