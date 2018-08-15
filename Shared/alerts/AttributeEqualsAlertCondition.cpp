
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

#include "AttributeEqualsAlertCondition.h"

// example app headers
#include "AlertConstants.h"
#include "AttributeEqualsAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::AttributeEqualsAlertCondition
  \inmodule Dsa
  \inherits AlertCondition
  \brief Represents an attribute condition which will be coninuosly monitored and will
  trigger an alert when a source object's attribute matches the target value.

  This condition will create new \l AttributeEqualsAlertConditionData to track source and target objects.
  */

/*!
  \brief Constructor taking an \l AlertLevel (\a level) the \a name of the condition,
  an \a attributeName, and an optional \a parent.
 */
AttributeEqualsAlertCondition::AttributeEqualsAlertCondition(AlertLevel level,
                                                             const QString& name,
                                                             const QString& attributeName,
                                                             QObject* parent):
  AlertCondition(level, name, parent),
  m_attributeName(attributeName)
{
}

/*!
  \brief Destructor.
 */
AttributeEqualsAlertCondition::~AttributeEqualsAlertCondition()
{
}

/*!
  \brief Creates a new \l AttributeEqualsAlertConditionData to track \a source and \a target objects.
 */
AlertConditionData* AttributeEqualsAlertCondition::createData(AlertSource* source, AlertTarget* target)
{
  return new AttributeEqualsAlertConditionData(newConditionDataName(), level(), source, target, m_attributeName, this);
}

/*!
  \brief Returns the query string component for this condition in the form "[MyAttribute] =".
 */
QString AttributeEqualsAlertCondition::queryString() const
{
  return QString("[%1] = ").arg(m_attributeName);
}

/*!
  \brief Returns a map of the variable components that make up the query for this condition.

  This condition type uses a query comprising the following components:

  \list
    \li attribute_name. The name of the attribute field to be queried.
  \endlist
 */
QVariantMap AttributeEqualsAlertCondition::queryComponents() const
{
  QVariantMap queryMap;
  queryMap.insert(AlertConstants::ATTRIBUTE_NAME, m_attributeName);

  return queryMap;
}

/*!
  \brief Static method to extract the attribute name from a \a queryMap.
 */
QString AttributeEqualsAlertCondition::attributeNameFromQueryComponents(const QVariantMap& queryMap)
{
  return queryMap.value(AlertConstants::ATTRIBUTE_NAME).toString();
}

} // Dsa
