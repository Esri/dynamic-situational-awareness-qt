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

#include "AbstractAlert.h"
#include "GraphicPairAlert.h"
#include "DistanceAlertRule.h"

#include "GeometryEngine.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

DistanceAlertRule::DistanceAlertRule(QObject* parent):
  AbstractAlertRule(parent)
{

}

DistanceAlertRule::~DistanceAlertRule()
{

}

bool DistanceAlertRule::matchesRule(AbstractAlert* alert) const
{
  if (!alert)
    return false;

  GraphicPairAlert* pairAlert = qobject_cast<GraphicPairAlert*>(alert);
  if (!pairAlert)
    return true; // test is not valid for this alert type

  Geometry geom1 = GeometryEngine::project(pairAlert->position(), SpatialReference::wgs84());
  Geometry geom2 = GeometryEngine::project(pairAlert->position2(), geom1.spatialReference());
  const double result = GeometryEngine::instance()->distance(geom1, geom2);

  if (result <= pairAlert->distance())
  {
    if (!pairAlert->active())
    {
      pairAlert->setActive(true);
    }

    return true;
  }
  else
  {
    pairAlert->setActive(false);
  }

  return false;
}
