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

#ifndef GEOELEMENTALERTTARGET_H
#define GEOELEMENTALERTTARGET_H

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
}
}

#include "AlertSpatialTarget.h"

class GeoElementAlertTarget : public AlertSpatialTarget
{
  Q_OBJECT

public:
  explicit GeoElementAlertTarget(Esri::ArcGISRuntime::GeoElement* geoElement);
  ~GeoElementAlertTarget();

  virtual QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;

private:
  Esri::ArcGISRuntime::GeoElement* m_geoElement = nullptr;
};

#endif // GEOELEMENTALERTTARGET_H
