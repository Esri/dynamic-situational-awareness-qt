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

#include "GeoElement.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

WithinDistanceAlertConditionData::WithinDistanceAlertConditionData(const QString& name,
                                                                   AlertLevel level,
                                                                   AlertSource* source,
                                                                   AlertTarget* target,
                                                                   double distance,
                                                                   QObject* parent):
  AlertConditionData(name, level, source, target, parent),
  m_spatialTarget(target),
  m_distance(distance)
{

}

WithinDistanceAlertConditionData::~WithinDistanceAlertConditionData()
{

}

double WithinDistanceAlertConditionData::distance() const
{
  return m_distance;
}

bool WithinDistanceAlertConditionData::matchesQuery() const
{
  const Geometry bufferGeom = GeometryEngine::bufferGeodetic(sourceLocation(), distance(), LinearUnit::meters(), 1.0, GeodeticCurveType::Geodesic);
  const Geometry bufferWgs84 = GeometryEngine::project(bufferGeom, SpatialReference::wgs84());

  const QList<Geometry> targetGeometries = spatialTarget()->targetGeometries(bufferWgs84.extent());

  for (const Geometry& target : targetGeometries)
  {
    Geometry targetWgs84 = GeometryEngine::project(target, SpatialReference::wgs84());
    if (GeometryEngine::intersects(bufferWgs84, targetWgs84))
      return true;
  }

  return false;
}
