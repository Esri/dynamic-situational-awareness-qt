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

#include "FixedValueAlertTarget.h"

// C++ API headers
#include "Geometry.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::FixedValueAlertTarget
  \inmodule Dsa
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

} // Dsa
