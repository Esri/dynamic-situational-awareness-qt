
/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

#include "GeoElementAlertTarget.h"

#include "GeoElementUtils.h"

// C++ API headers
#include "GeoElement.h"
#include "Geometry.h"
#include "GeometryEngine.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::GeoElementAlertTarget
  \inmodule Dsa
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
  AlertTarget(GeoElementUtils::toQObject(geoElement)),
  m_geoElementSignaler(new GeoElementSignaler(geoElement, this))
{
  connect(m_geoElementSignaler, &GeoElementSignaler::geometryChanged, this, &GeoElementAlertTarget::dataChanged);
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
  return QList<Geometry>{m_geoElementSignaler->m_geoElement->geometry()};
}

/*!
  \brief Returns an empty QVariant.
 */
QVariant GeoElementAlertTarget::targetValue() const
{
  return QVariant();
}

} // Dsa
