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

#include "AlertQuery.h"
#include "StatusAlertQuery.h"

StatusAlertQuery::StatusAlertQuery(QObject* parent):
  AlertQuery(parent)
{

}

StatusAlertQuery::~StatusAlertQuery()
{

}

bool StatusAlertQuery::matchesRule(AlertConditionData* alert) const
{
  if (!alert)
    return false;

  return alert->level() >= m_minLevel;
}

AlertLevel StatusAlertQuery::minLevel() const
{
  return m_minLevel;
}

void StatusAlertQuery::setMinLevel(AlertLevel minStatus)
{
  m_minLevel = minStatus;
}
