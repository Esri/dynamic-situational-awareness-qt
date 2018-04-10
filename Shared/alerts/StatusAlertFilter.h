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

#ifndef STATUSALERTFILTER_H
#define STATUSALERTFILTER_H

// example app headers
#include "AlertConditionData.h"
#include "AlertFilter.h"

namespace Dsa {

class AlertConditionData;

class StatusAlertFilter : public AlertFilter
{
  Q_OBJECT

public:
  explicit StatusAlertFilter(QObject* parent = nullptr);
  ~StatusAlertFilter();

  bool passesFilter(AlertConditionData* alert) const override;

  AlertLevel minLevel() const;
  void setMinLevel(AlertLevel minLevel);

private:
  AlertLevel m_minLevel = AlertLevel::Low;
};

} // Dsa

#endif // STATUSALERTFILTER_H
