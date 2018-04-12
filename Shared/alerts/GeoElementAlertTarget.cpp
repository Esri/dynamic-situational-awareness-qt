// Copyright 2018 ESRI
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

// PCH header
#include "pch.hpp"

#include "GeoElementAlertTarget.h"

// C++ API headers
#include "GeoElement.h"
#include "Geometry.h"
#include "GeometryEngine.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

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
QList<Geometry> GeoElementAlertTarget::targetGeometries(const Envelope&) const
{
  return QList<Geometry>{m_geoElement->geometry()};
}

/*!
  \brief Returns an empty QVariant.
 */
QVariant GeoElementAlertTarget::targetValue() const
{
  return QVariant();
}

} // Dsa
