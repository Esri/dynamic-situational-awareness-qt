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

#ifndef INTERSECTSALERTRULE_H
#define INTERSECTSALERTRULE_H

#include "AbstractAlertRule.h"

class AlertConditionData;

class IntersectsAlertRule : public AlertQuery
{
  Q_OBJECT

public:
  explicit IntersectsAlertRule(QObject* parent = nullptr);
  ~IntersectsAlertRule();

  bool matchesRule(AlertConditionData* alert) const override;
};

#endif // INTERSECTSALERTRULE_H
