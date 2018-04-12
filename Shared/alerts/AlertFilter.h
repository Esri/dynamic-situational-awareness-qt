// Copyright 2018 ESRI
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

#ifndef ALERTFILTER_H
#define ALERTFILTER_H

// Qt headers
#include <QObject>

namespace Dsa {

class AlertConditionData;

class AlertFilter : public QObject
{
  Q_OBJECT

public:
  explicit AlertFilter(QObject* parent = nullptr);
  ~AlertFilter();

  virtual bool passesFilter(AlertConditionData* alertData) const = 0;

signals:
  void filterChanged();
};

} // Dsa

#endif // ALERTFILTER_H
