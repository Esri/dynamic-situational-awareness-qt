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

#include "AlertFilter.h"
#include "StatusAlertFilter.h"

StatusAlertFilter::StatusAlertFilter(QObject* parent):
  AlertFilter(parent)
{

}

StatusAlertFilter::~StatusAlertFilter()
{

}

bool StatusAlertFilter::passesFilter(AlertConditionData* alert) const
{
  if (!alert)
    return false;

  return alert->level() >= m_minLevel;
}

AlertLevel StatusAlertFilter::minLevel() const
{
  return m_minLevel;
}

void StatusAlertFilter::setMinLevel(AlertLevel minStatus)
{
  m_minLevel = minStatus;
}
