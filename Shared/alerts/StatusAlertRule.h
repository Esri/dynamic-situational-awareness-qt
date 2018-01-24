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

#ifndef STATUSALERTRULE_H
#define STATUSALERTRULE_H

#include "AlertConditionData.h"
#include "AbstractAlertRule.h"

class AlertConditionData;

class StatusAlertRule : public AlertQuery
{
  Q_OBJECT

public:
  explicit StatusAlertRule(QObject* parent = nullptr);
  ~StatusAlertRule();

  bool matchesRule(AlertConditionData* alert) const override;

  AlertLevel minStatus() const;
  void setMinStatus(const AlertLevel& minStatus);

private:
  AlertLevel m_minStatus = AlertLevel::Low;
};

#endif // STATUSALERTRULE_H
