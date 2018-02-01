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

#ifndef ALERTSPATIALTARGET_H
#define ALERTSPATIALTARGET_H

#include "Geometry.h"

#include "AlertTarget.h"

#include <QList>

namespace Esri
{
namespace ArcGISRuntime
{
  class Envelope;
}
}

class AlertSpatialTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit AlertSpatialTarget(QObject* parent = nullptr);
  ~AlertSpatialTarget();

  virtual QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const = 0;
};

#endif // ALERTSPATIALTARGET_H
