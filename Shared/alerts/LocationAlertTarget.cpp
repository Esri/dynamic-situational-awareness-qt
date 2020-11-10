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

#include "LocationAlertTarget.h"

// toolkit headers
#include "ToolResourceProvider.h"

// C++ API headers
#include "GeometryEngine.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::LocationAlertTarget
  \inmodule Dsa
  \inherits AlertTarget
  \brief Represents a target based on the current location.

  Changes to the underlying location will cause the \l AlertTarget::locationChanged
  signal to be emitted.
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
LocationAlertTarget::LocationAlertTarget(QObject* parent):
  AlertTarget(parent)
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::locationChanged, this, [this](const Point& location)
  {
    if (m_location == location)
      return;

    m_location = location;
    emit dataChanged();
  });
}

/*!
  \brief Destructor.
 */
LocationAlertTarget::~LocationAlertTarget()
{

}

/*!
  \brief Returns a list of \l Esri::ArcGISRuntime::Geometry within the \a targetArea.

  \note This will contain one element for the current location if it is within the target area.
 */
QList<Geometry> LocationAlertTarget::targetGeometries(const Envelope& targetArea) const
{
  const Point projected = GeometryEngine::project(m_location, targetArea.spatialReference());
  if (GeometryEngine::instance()->contains(targetArea, projected))
    return QList<Geometry>{m_location};

  return QList<Geometry>();
}

/*!
  \brief Returns the underlying value.

  \note This will be empty since this is a spatial type.
 */
QVariant LocationAlertTarget::targetValue() const
{
  return QVariant();
}


} // Dsa
