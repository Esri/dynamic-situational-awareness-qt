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

bool WithinDistanceAlertQuery::matchesRule(AlertConditionData* alert) const
{
  if (!alert)
    return false;

  WithinDistanceAlertConditionData* pairAlert = qobject_cast<WithinDistanceAlertConditionData*>(alert);
  if (!pairAlert)
    return true; // test is not valid for this alert type

  Point sourceGeom = GeometryEngine::project(pairAlert->sourcePosition(), SpatialReference::wgs84());
  Geometry geom2 = GeometryEngine::project(pairAlert->position2(), sourceGeom.spatialReference());
  Point nearestPoint;

  switch (geom2.geometryType())
  {
  case GeometryType::Point:
    nearestPoint = geom2;
    break;
  case GeometryType::Polyline:
  case GeometryType::Polygon:
  {
    nearestPoint = GeometryEngine::nearestCoordinate(geom2, sourceGeom).coordinate();
    break;
  }
  default:
    nearestPoint = geom2.extent().center();
    break;
  }

  const GeodeticDistanceResult result = GeometryEngine::instance()->distanceGeodetic(
        sourceGeom, nearestPoint, LinearUnit::meters(), AngularUnit::degrees(), GeodeticCurveType::Geodesic);

  return result.distance() <= pairAlert->distance();
}
