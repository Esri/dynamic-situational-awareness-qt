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

#include "AlertCondition.h"
#include "AlertConditionData.h"

using namespace Esri::ArcGISRuntime;

AlertConditionData::AlertConditionData(AlertCondition* parent):
  QObject(parent),
  m_condition(parent)
{
  connect(m_condition, &AlertCondition::noLongerValid, this, &AlertConditionData::noLongerValid);
}

AlertConditionData::~AlertConditionData()
{
  emit noLongerValid();
}

AlertLevel AlertConditionData::level() const
{
  return m_condition->level();
}

QString AlertConditionData::name() const
{
  return m_condition->name();
}

QUuid AlertConditionData::id() const
{
  return m_id;
}

void AlertConditionData::setId(const QUuid& id)
{
  m_id = id;
}

bool AlertConditionData::viewed() const
{
  return m_viewed;
}

void AlertConditionData::setViewed(bool viewed)
{
  if (viewed == m_viewed)
    return;

  m_viewed = viewed;
  emit viewedChanged();
}

void AlertConditionData::onPositionChanged()
{
  emit positionChanged();
}

void AlertConditionData::setActive(bool active)
{
  if (active == m_active)
    return;

  m_active = active;
  emit activeChanged();
}

bool AlertConditionData::active() const
{
  return m_active;
}
