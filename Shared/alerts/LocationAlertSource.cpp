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

#include "LocationAlertSource.h"

#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class LocationAlertSource
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
  AlertSource(parent)
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged, this, [this](const Point& location)
  {
    m_location = location;
    emit locationChanged();
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
  \brief This method is not implemented for this type.
 */
void LocationAlertSource::setSelected(bool)
{
  // do not select the location display
}
