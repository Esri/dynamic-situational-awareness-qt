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
