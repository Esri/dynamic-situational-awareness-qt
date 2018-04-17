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
