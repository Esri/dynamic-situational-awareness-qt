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

#include "WithinDistanceAlertCondition.h"
#include "WithinDistanceAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

WithinDistanceAlertCondition::WithinDistanceAlertCondition(AlertLevel level,
                                                           const QString& name,
                                                           double distance,
                                                           QObject* parent):
  AlertCondition(level, name, parent),
  m_distance(distance)
{

}

WithinDistanceAlertCondition::~WithinDistanceAlertCondition()
{

}

AlertConditionData* WithinDistanceAlertCondition::createData(AlertSource* source, AlertTarget* target)
{
  return new WithinDistanceAlertConditionData(this, source, target, m_distance);
}

double WithinDistanceAlertCondition::distance() const
{
  return m_distance;
}
