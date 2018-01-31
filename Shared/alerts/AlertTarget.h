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

#ifndef ALERTTARGET_H
#define ALERTTARGET_H

#include "Geometry.h"

#include <QObject>

#include <QList>

namespace Esri
{
namespace ArcGISRuntime
{
  class Envelope;
}
}

class AlertTarget : public QObject
{
  Q_OBJECT

public:
  explicit AlertTarget(QObject* parent = nullptr);
  ~AlertTarget();

  virtual QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const = 0;

signals:
  void locationChanged();
  void noLongerValid();
};

#endif // ALERTTARGET_H