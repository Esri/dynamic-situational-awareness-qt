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

#include "AbstractAlertRule.h"
#include "AbstractAlert.h"

class StatusAlertRule : public AbstractAlertRule
{
  Q_OBJECT

public:
  explicit StatusAlertRule(QObject* parent = nullptr);
  ~StatusAlertRule();

  bool matchesRule(AbstractAlert* alert) const override;

  AlertStatus minStatus() const;
  void setMinStatus(const AlertStatus& minStatus);

private:
  AlertStatus m_minStatus = AlertStatus::Low;
};

#endif // STATUSALERTRULE_H
