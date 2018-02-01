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

#include "AlertConditionData.h"
#include "IdsAlertFilter.h"

IdsAlertFilter::IdsAlertFilter(QObject* parent):
  AlertFilter(parent)
{

}

IdsAlertFilter::~IdsAlertFilter()
{

}

bool IdsAlertFilter::passesFilter(AlertConditionData* alert) const
{
  if (!alert)
    return false;

  return !m_ids.contains(alert->id());
}

void IdsAlertFilter::addId(const QUuid& id)
{
  m_ids.insert(id);
}

