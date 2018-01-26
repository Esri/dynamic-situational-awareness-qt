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

#ifndef LOCATIONALERTSOURCE_H
#define LOCATIONALERTSOURCE_H

#include "AlertSource.h"

class LocationAlertSource : public AlertSource
{
  Q_OBJECT

public:
  explicit LocationAlertSource(QObject* parent = nullptr);
  ~LocationAlertSource();

  Esri::ArcGISRuntime::Point position() const override;

private:
  Esri::ArcGISRuntime::Point m_location;
};

#endif // LOCATIONALERTSOURCE_H
