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
#include "AlertListModel.h"

using namespace Esri::ArcGISRuntime;


AlertCondition::AlertCondition(const AlertLevel& level, const QString& name, QObject* parent):
  QObject(parent),
  m_level(level),
  m_name(name)
{

}

AlertCondition::~AlertCondition()
{

}

AlertLevel AlertCondition::level() const
{
  return m_level;
}

void AlertCondition::alertLevel(const AlertLevel& level)
{
  if (level == m_level)
    return;

  m_level = level;
}

QString AlertCondition::name() const
{
  return m_name;
}

void AlertCondition::setName(const QString& name)
{
  if (name == m_name)
    return;

  m_name = name;
}

void AlertCondition::addData(AlertConditionData* newData)
{
  if (!newData)
    return;

  AlertListModel::instance()->addAlertConditionData(newData);
}
