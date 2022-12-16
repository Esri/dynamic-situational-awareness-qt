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

#include "WithinAreaAlertCondition.h"

// dsa app headers
#include "WithinAreaAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::WithinAreaAlertCondition
  \inmodule Dsa
  \inherits AlertCondition
  \brief Represents a spatial, "Geofence", condition which will be coninuosly monitored and will
  trigger an alert when a source object is within the area of a target object.

  This condition will create new \l WithinAreaAlertConditionData to track source and target objects.

  It can be tested using a \l WithinAreaAlertQuery.
  */

/*!
  \brief Constructor taking an \l AlertLevel (\a level) the \a name of the condition and
  an optional \a parent.
 */
WithinAreaAlertCondition::WithinAreaAlertCondition(AlertLevel level,
                                                   const QString& name,
                                                   QObject* parent):
  AlertCondition(level, name, parent)
{

}

/*!
  \brief Destructor.
 */
WithinAreaAlertCondition::~WithinAreaAlertCondition()
{

}

/*!
  \brief Creates a new \l WithinAreaAlertConditionData to track \a source and \a target objects.
 */
AlertConditionData* WithinAreaAlertCondition::createData(AlertSource* source, AlertTarget* target)
{
  return new WithinAreaAlertConditionData(newConditionDataName(), level(), source, target, this);
}

/*!
  \brief Returns the query string component for this condition - e.g. "is within".
 */
QString WithinAreaAlertCondition::queryString() const
{
  return isWithinQueryString();
}

/*!
  \brief Returns a map of the variable components that make up the query for this condition.

  \note This query type has no variable components.
 */
QVariantMap WithinAreaAlertCondition::queryComponents() const
{
  return QVariantMap();
}

/*!
  \brief Static method to get the query string "is within".
 */
QString WithinAreaAlertCondition::isWithinQueryString()
{
  return QStringLiteral("is within");
}


} // Dsa
