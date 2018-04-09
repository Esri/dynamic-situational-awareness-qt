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

#include "WithinAreaAlertConditionData.h"

// example app headers
#include "AlertSource.h"
#include "AlertTarget.h"

// C++ API headers
#include "GeoElement.h"
#include "GeometryEngine.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa
{
namespace Alerts
{

/*!
  \class WithinAreaAlertConditionData
  \inherits AlertConditionData
  \brief Represents the data to be tested as part of a spatial, within area condition.

  This condition data allows a query to determine whether a source object is within the area
  of a target object, or objects.

  The target should be a polygon geoemtry type.
 */

/*!
  \brief Constructor for a new within area condition data object.

  \list
    \li \a name. The name of the condition.
    \li \a level. The \l AlertLevel for the condition.
    \li \a source. The source data for the condition (for example
      \l Esri::ArcGISRuntime::Graphic or a location).
    \li \a target. The target data for the condition. For example,
      a real-time feed or an overlay.
    \li \a parent. The (optional) parent object.
  \endlist
 */
WithinAreaAlertConditionData::WithinAreaAlertConditionData(const QString& name,
                                                           AlertLevel level,
                                                           AlertSource* source,
                                                           AlertTarget* target,
                                                           QObject* parent):
  AlertConditionData(name, level, source, target, parent)
{

}

/*!
  \brief Destructor.
 */
WithinAreaAlertConditionData::~WithinAreaAlertConditionData()
{

}

/*!
  \brief Returns whether the source data currently lies within the target object or objects.
 */
bool WithinAreaAlertConditionData::matchesQuery() const
{
  if (!isQueryOutOfDate())
    return cachedQueryResult();

  Geometry sourceWgs84 = GeometryEngine::project(sourceLocation(), SpatialReference::wgs84());
  const QList<Geometry> targetGeometries = target()->targetGeometries(sourceWgs84.extent());

  for (const Geometry& target : targetGeometries)
  {
    if (target.geometryType() != GeometryType::Polygon)
      continue;

    const Geometry targetWgs84 = GeometryEngine::project(target, sourceWgs84.spatialReference());
    if (GeometryEngine::instance()->intersects(sourceWgs84, targetWgs84))
      return true;
  }

  return false;
}

} // Alerts
} // Dsa
