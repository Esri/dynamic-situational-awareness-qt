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

#include "AlertConditionData.h"
#include "AlertTarget.h"
#include "WithinDistanceAlertConditionData.h"
#include "WithinDistanceAlertQuery.h"

#include "GeometryEngine.h"

using namespace Esri::ArcGISRuntime;

WithinDistanceAlertQuery::WithinDistanceAlertQuery(QObject* parent):
  AlertQuery(parent)
{
}

WithinDistanceAlertQuery::~WithinDistanceAlertQuery()
{

}

bool WithinDistanceAlertQuery::matchesRule(AlertConditionData* conditionData) const
{
  if (!conditionData)
    return false;

  WithinDistanceAlertConditionData* withinDistanceData = qobject_cast<WithinDistanceAlertConditionData*>(conditionData);
  if (!withinDistanceData)
    return true; // test is not valid for this condition

  const Geometry bufferGeom = GeometryEngine::bufferGeodetic(withinDistanceData->sourceLocation(), withinDistanceData->distance(), LinearUnit::meters(), 1.0, GeodeticCurveType::Geodesic);
  const Geometry bufferWgs84 = GeometryEngine::project(bufferGeom, SpatialReference::wgs84());

  const QList<Geometry> targetGeometries = withinDistanceData->target()->targetGeometries(bufferWgs84.extent());

  for (const Geometry& target : targetGeometries)
  {
    Geometry targetWgs84 = GeometryEngine::project(target, SpatialReference::wgs84());
    if (GeometryEngine::intersects(bufferWgs84, targetWgs84))
      return true;
  }

  return false;
}
