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

#include "StatusAlertRule.h"

StatusAlertRule::StatusAlertRule(QObject* parent):
  AbstractAlertRule(parent)
{

}

StatusAlertRule::~StatusAlertRule()
{

}

bool StatusAlertRule::matchesRule(AbstractAlert* alert) const
{
  if (!alert)
    return false;

  return alert->status() >= m_minStatus;
}

AlertStatus StatusAlertRule::minStatus() const
{
  return m_minStatus;
}

void StatusAlertRule::setMinStatus(const AlertStatus& minStatus)
{
  m_minStatus = minStatus;
}
