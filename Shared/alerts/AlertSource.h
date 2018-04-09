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


// C++ API headers
#include "Point.h"

// Qt headers
#include <QObject>
#include <QVariant>

namespace Dsa {
namespace Alerts {

class AlertSource : public QObject
{
  Q_OBJECT

public:
  explicit AlertSource(QObject* parent = nullptr);
  ~AlertSource();

  virtual Esri::ArcGISRuntime::Point location() const = 0;
  virtual QVariant value(const QString& key) const = 0;

  virtual void setSelected(bool selected) = 0;

signals:
  void dataChanged();
  void noLongerValid();
};

} // Alerts
} // Dsa

#endif // ALERTSOURCE_H
