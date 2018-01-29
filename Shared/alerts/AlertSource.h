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

#ifndef ALERTSOURCE_H
#define ALERTSOURCE_H

#include "Point.h"

#include <QObject>

class AlertSource : public QObject
{
  Q_OBJECT

public:
  explicit AlertSource(QObject* parent = nullptr);
  ~AlertSource();

  virtual Esri::ArcGISRuntime::Point location() const = 0;
  virtual void setSelected(bool selected) = 0;

signals:
  void locationChanged();
  void noLongerValid();
};

#endif // ALERTSOURCE_H
