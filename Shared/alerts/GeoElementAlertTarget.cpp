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

/*!
  \class GeoElementAlertTarget
  \inherits AlertTarget
  \brief Represents a target based on a single \l Esri::ArcGISRuntime::GeoElement
  for an \l AlertCondition.

  Changes to the geometry of the underlying element will cause the \l AlertTarget::locationChanged
  signal to be emitted.
  */

/*!
  \brief Constructor taking an \l Esri::ArcGISRuntime::GeoElement (\a geoElement).
 */
GeoElementAlertTarget::GeoElementAlertTarget(GeoElement* geoElement):
  AlertTarget(geoElement),
  m_geoElement(geoElement)
{
  connect(m_geoElement, &GeoElement::geometryChanged, this, &GeoElementAlertTarget::dataChanged);
}

/*!
  \brief Destructor.
 */
GeoElementAlertTarget::~GeoElementAlertTarget()
{

}

/*!
  \brief Returns the \l Esri::ArcGISRuntime::Geometry of the underlying \l \l Esri::ArcGISRuntime::GeoElement.

  \note No exact intersection tests are carried against the \a targetArea for this type.
 */
QList<Esri::ArcGISRuntime::Geometry> GeoElementAlertTarget::targetGeometries(const Esri::ArcGISRuntime::Envelope&) const
{
  return QList<Geometry>{m_geoElement->geometry()};
}
