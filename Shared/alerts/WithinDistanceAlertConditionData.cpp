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

#include "WithinDistanceAlertConditionData.h"
#include "AlertSource.h"
#include "AlertTarget.h"

#include "GeoElement.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class WithinDistanceAlertConditionData
  \inherits AlertConditionData
  \brief Represents the data to be tested as part of a spatial, within distance condition.

  This condition data allows a query to determine whether a source object is within a threshold
  distance of a target object, or objects.
 */

/*!
  \brief Constructor for a new within distance condition data object.

  \list
    \li \a name. The name of the condition.
    \li \a level. The \l AlertLevel for the condition.
    \li \a source. The source data for the condition (for example
      \l Esri::ArcGISRuntime::Graphic or a location).
    \li \a target. The target data for the condition. For example,
      a real-time feed or an overlay.
    \li \a distance. The threshold distance in meters.
    \li \a parent. The (optional) parent object.
  \endlist
 */
WithinDistanceAlertConditionData::WithinDistanceAlertConditionData(const QString& name,
                                                                   AlertLevel level,
                                                                   AlertSource* source,
                                                                   AlertTarget* target,
                                                                   double distance,
                                                                   QObject* parent):
  AlertConditionData(name, level, source, target, parent),
  m_distance(distance)
{

}

/*!
  \brief Destructor.
 */
WithinDistanceAlertConditionData::~WithinDistanceAlertConditionData()
{

}

/*!
  \brief Returns the threshold distance in meters.
 */
double WithinDistanceAlertConditionData::distance() const
{
  return m_distance;
}

/*!
  \brief Returns whether the source data currently lies within the threshold distance of
  the target object or objects.
 */
bool WithinDistanceAlertConditionData::matchesQuery() const
{
  if (!isQueryOutOfDate())
    return cachedQueryResult();

  const Geometry bufferGeom = GeometryEngine::bufferGeodetic(sourceLocation(), distance(), LinearUnit::meters(), 1.0, GeodeticCurveType::Geodesic);
  const Geometry bufferWgs84 = GeometryEngine::project(bufferGeom, SpatialReference::wgs84());

  const QList<Geometry> targetGeometries = target()->targetGeometries(bufferWgs84.extent());

  for (const Geometry& target : targetGeometries)
  {
    Geometry targetWgs84 = GeometryEngine::project(target, SpatialReference::wgs84());
    if (GeometryEngine::intersects(bufferWgs84, targetWgs84))
      return true;
  }

  return false;
}
