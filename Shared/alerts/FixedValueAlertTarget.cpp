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

// PCH header
#include "pch.hpp"

#include "FixedValueAlertTarget.h"

// C++ API headers
#include "GeoElement.h"
#include "Geometry.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa
{
namespace Alerts
{

/*!
  \class FixedValueAlertTarget
  \inherits AlertTarget
  \brief Represents a target based on a fixed value for an \l AlertCondition.

  Changes to the underlying value will cause the \l AlertTarget::locationChanged
  signal to be emitted.
  */

/*!
  \brief Constructor taking a \a value and an optional \a parent.
 */
FixedValueAlertTarget::FixedValueAlertTarget(const QVariant& value, QObject* parent):
  AlertTarget(parent),
  m_value(value)
{

}

/*!
  \brief Destructor.
 */
FixedValueAlertTarget::~FixedValueAlertTarget()
{

}

/*!
  \brief Returns an empty list of \l Esri::ArcGISRuntime::Geometry.

  \note This type of target is non-spatial.
 */
QList<Geometry> FixedValueAlertTarget::targetGeometries(const Envelope&) const
{
  return QList<Geometry>();
}

/*!
  \brief Returns the underlying value.
 */
QVariant FixedValueAlertTarget::targetValue() const
{
  return m_value;
}

} // Alerts
} // Dsa
