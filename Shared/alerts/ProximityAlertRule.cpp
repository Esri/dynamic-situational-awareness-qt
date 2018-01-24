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
#include "ProximityPairAlert.h"
#include "ProximityAlertRule.h"

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

  Geometry geom1 = GeometryEngine::project(pairAlert->position(), SpatialReference::wgs84());
  Geometry geom2 = GeometryEngine::project(pairAlert->position2(), geom1.spatialReference());
  const double result = GeometryEngine::instance()->distance(geom1, geom2);

  return result <= pairAlert->distance();
}
