
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

#include "LocationAlertSource.h"

// toolkit headers
#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::LocationAlertSource
  \inmodule Dsa
  \inherits AlertSource
  \brief Represents a source, based on a the current location of the device,
  for an \l AlertCondition.

  Changes to the device position will cause the \l AlertSource::locationChanged
  signal to be emitted.

  /sa Toolkit::ToolResourceProvider
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
LocationAlertSource::LocationAlertSource(QObject* parent):
  AlertSource(parent),
  m_location(0., 0., SpatialReference::wgs84())
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged, this, [this](const Point& location)
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
LocationAlertSource::~LocationAlertSource()
{
}

/*!
  \brief Returns the current location of the device.
 */
Point LocationAlertSource::location() const
{
  return m_location;
}

/*!
  \brief Returns an empty QVariant since this type has no attributes.
 */
QVariant LocationAlertSource::value(const QString&) const
{
  return QVariant();
}

/*!
  \brief This method is not implemented for this type.
 */
void LocationAlertSource::setSelected(bool)
{
  // do not select the location display
}

} // Dsa
