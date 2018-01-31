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

#include "AlertTarget.h"
#include "WithinAreaAlertConditionData.h"
#include "WithinAreaAlertQuery.h"

#include "GeometryEngine.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

WithinAreaAlertQuery::WithinAreaAlertQuery(QObject* parent):
  AlertQuery(parent)
{

}

WithinAreaAlertQuery::~WithinAreaAlertQuery()
{

}

bool WithinAreaAlertQuery::matchesRule(AlertConditionData* conditionData) const
{
  if (!conditionData)
    return false;

  WithinAreaAlertConditionData* withinAreaConditionData = qobject_cast<WithinAreaAlertConditionData*>(conditionData);
  if (!withinAreaConditionData)
    return true; // test is not valid for this alert type

  Geometry sourceWgs84 = GeometryEngine::project(withinAreaConditionData->sourceLocation(), SpatialReference::wgs84());
  const QList<Geometry> targetGeometries = withinAreaConditionData->target()->targetGeometries(sourceWgs84.extent());

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
