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

#include "GeoElementAlertTarget.h"

#include "GeoElement.h"

using namespace Esri::ArcGISRuntime;

GeoElementAlertTarget::GeoElementAlertTarget(GeoElement* geoElement):
  AlertTarget(geoElement),
  m_geoElement(geoElement)
{
  connect(m_geoElement, &GeoElement::geometryChanged, this, &GeoElementAlertTarget::locationChanged);
}

GeoElementAlertTarget::~GeoElementAlertTarget()
{

}

QList<Esri::ArcGISRuntime::Geometry> GeoElementAlertTarget::location() const
{
  return QList<Geometry>{m_geoElement->geometry()};
}
