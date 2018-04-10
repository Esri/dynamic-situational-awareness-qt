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

#ifndef LOCATIONALERTTARGET_H
#define LOCATIONALERTTARGET_H

// example app headers
#include "AlertTarget.h"

// C++ API headers
#include "Point.h"

namespace Dsa {

class LocationAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit LocationAlertTarget(QObject* parent = nullptr);
  ~LocationAlertTarget();

  QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;
  QVariant targetValue() const override;

private:
  Esri::ArcGISRuntime::Point m_location;
};

} // Dsa

#endif // LOCATIONALERTTARGET_H
