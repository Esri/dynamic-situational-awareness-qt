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

#ifndef STATUSALERTQUERY_H
#define STATUSALERTQUERY_H

#include "AlertConditionData.h"
#include "AlertQuery.h"

class AlertConditionData;

class StatusAlertQuery : public AlertQuery
{
  Q_OBJECT

public:
  explicit StatusAlertQuery(QObject* parent = nullptr);
  ~StatusAlertQuery();

  bool matchesRule(AlertConditionData* alert) const override;

  AlertLevel minLevel() const;
  void setMinLevel(const AlertLevel& minLevel);

private:
  AlertLevel m_minLevel = AlertLevel::Low;
};

#endif // STATUSALERTQUERY_H
