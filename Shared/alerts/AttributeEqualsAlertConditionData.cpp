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

#include "AlertSource.h"
#include "AlertTarget.h"
#include "AttributeEqualsAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

AttributeEqualsAlertConditionData::AttributeEqualsAlertConditionData(const QString& name,
                                                                     AlertLevel level,
                                                                     AlertSource* source,
                                                                     AlertTarget* target,
                                                                     const QString& attributeName,
                                                                     QObject* parent):
  AlertConditionData(name, level, source, target, parent),
  m_attributeName(attributeName)
{

}

AttributeEqualsAlertConditionData::~AttributeEqualsAlertConditionData()
{

}

bool AttributeEqualsAlertConditionData::matchesQuery() const
{
  const QVariant sourceValue = source()->value(attributeName());
  if (sourceValue.isNull() || !sourceValue.isValid())
    return false;

  const QVariant targetValue = target()->targetValue();
  if (targetValue.isNull() || !targetValue.isValid())
    return false;

  return sourceValue == targetValue;
}

QString AttributeEqualsAlertConditionData::attributeName() const
{
    return m_attributeName;
}
