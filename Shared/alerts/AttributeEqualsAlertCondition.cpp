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

#include "AttributeEqualsAlertCondition.h"
#include "AttributeEqualsAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class AttributeEqualsAlertCondition
  \inherits AlertCondition
  \brief Represents an attribute condition which will be coninuosly monitored and will
  trigger an alert when a source object's attribute matches the target value.

  This condition will create new \l AttributeEqualsAlertConditionData to track source and target objects.
  */

/*!
  \brief Constructor taking an \l AlertLevel (\a level) the \a name of the condition and
  an optional \a parent.
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
  \brief Creates a new \l AttributeEqualsAlertConditionData to track source and target objects.
 */
AlertConditionData* AttributeEqualsAlertCondition::createData(AlertSource* source, AlertTarget* target)
{
  return new AttributeEqualsAlertConditionData(newConditionDataName(), level(), source, target, m_attributeName, this);
}
