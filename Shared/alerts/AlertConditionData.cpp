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

using namespace Esri::ArcGISRuntime;

AlertConditionData::AlertConditionData(QObject* parent):
  QObject(parent)
{
}

AlertConditionData::~AlertConditionData()
{

}

AlertLevel AlertConditionData::level() const
{
  return m_level;
}

void AlertConditionData::alertLevel(const AlertLevel& status)
{
  if (status == m_level)
    return;

  m_level = status;
  emit statusChanged();
}

QString AlertConditionData::name() const
{
  return m_name;
}

void AlertConditionData::setName(const QString& message)
{
  m_name = message;
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
