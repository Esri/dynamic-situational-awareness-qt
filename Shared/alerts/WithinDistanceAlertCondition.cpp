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

#include "WithinDistanceAlertCondition.h"

// example app headers
#include "AlertConstants.h"
#include "WithinDistanceAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class WithinDistanceAlertCondition
  \inherits AlertCondition
  \brief Represents a spatial, "Geofence", condition which will be coninuosly monitored and will
  trigger an alert when a source object is within a threshold distance of a target object.

  This condition will create new \l WithinDistanceAlertConditionData to track source and target objects.

  It can be tested using a \l WithinDistanceAlertQuery.
  */

/*!
  \brief Constructor taking an \l AlertLevel (\a level) the \a name of the condition,
  the threshold distance (in meters) and an optional \a parent.
 */
WithinDistanceAlertCondition::WithinDistanceAlertCondition(AlertLevel level,
                                                           const QString& name,
                                                           double distance,
                                                           QObject* parent):
  AlertCondition(level, name, parent),
  m_distance(distance)
{

}

/*!
  \brief Destructor.
 */
WithinDistanceAlertCondition::~WithinDistanceAlertCondition()
{

}

/*!
  \brief Creates a new \l WithinDistanceAlertConditionData to track source and target objects.
 */
AlertConditionData* WithinDistanceAlertCondition::createData(AlertSource* source, AlertTarget* target)
{
  return new WithinDistanceAlertConditionData(newConditionDataName(), level(), source, target, m_distance, this);
}

/*!
  \brief The threshold distance (in meters) for this condition.
 */
double WithinDistanceAlertCondition::distance() const
{
  return m_distance;
}

/*!
  \brief Returns a map of the variable components that make up the query for this condition.

  This condition type uses a query comprising the following components:

  \list
    \li distance. The threshold distance in meters.
  \endlist
 */
QVariantMap WithinDistanceAlertCondition::queryComponents() const
{
  QVariantMap queryMap;
  queryMap.insert(AlertConstants::METERS, m_distance);

  return queryMap;
}

/*!
  \brief Static method to get the distance in meters from a \a queryComponents.

  Returns \c -1.0 if unsuccessful
 */
double WithinDistanceAlertCondition::getDistanceFromQueryComponents(const QVariantMap& queryComponents)
{
  return queryComponents.value(AlertConstants::METERS, -1.0).toDouble();
}

/*!
  \brief Returns the query string component for this condition - e.g. "is within X meters of".
 */
QString WithinDistanceAlertCondition::queryString() const
{
  return QString("is within %1 %2 of").arg(QString::number(m_distance), AlertConstants::METERS);
}


} // Dsa
