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

#include "WithinAreaAlertConditionData.h"

// dsa app headers
#include "AlertSource.h"
#include "AlertTarget.h"

// C++ API headers
#include "Envelope.h"
#include "GeometryEngine.h"
#include "Point.h"
#include "SpatialReference.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::WithinAreaAlertConditionData
  \inmodule Dsa
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


} // Dsa
