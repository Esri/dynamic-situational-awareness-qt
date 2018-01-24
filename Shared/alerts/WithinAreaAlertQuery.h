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

#ifndef WITHINAREAALERTQUERY_H
#define WITHINAREAALERTQUERY_H

#include "AlertQuery.h"

class AlertConditionData;

class WithinAreaAlertQuery : public AlertQuery
{
  Q_OBJECT

public:
  explicit WithinAreaAlertQuery(QObject* parent = nullptr);
  ~WithinAreaAlertQuery();

  bool matchesRule(AlertConditionData* alert) const override;
};

#endif // WITHINAREAALERTQUERY_H
