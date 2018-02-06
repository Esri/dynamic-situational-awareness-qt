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

#include "WithinDistanceAlertCondition.h"
#include "WithinDistanceAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

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
  \brief Static methofd to get the distance in meters from a \a queryString.

  Returns \c -1.0 if unsuccessful
 */
double WithinDistanceAlertCondition::getDistanceFromQueryString(const QString& queryString)
{
  if (queryString.length() < 22)
    return -1.0;

  const int metersIndex = queryString.indexOf(QStringLiteral("meters"));
  if (metersIndex == -1 || metersIndex < 12)
    return -1.0;

  const QString distanceString = queryString.mid(10, (metersIndex - 2) - 10);
  bool ok = false;
  const double distance = distanceString.toDouble(&ok);

  return ok ? distance :-1.0;
}

/*!
  \brief Returns the query string component for this condition - e.g. "is within X meters of".
 */
QString WithinDistanceAlertCondition::queryString() const
{
  return QString("is within %2 meters of").arg(QString::number(m_distance));
}
