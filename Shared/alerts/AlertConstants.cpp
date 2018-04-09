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

#include "AlertConstants.h"

// PCH header
#include "pch.hpp"

// example app headers
#include "AttributeEqualsAlertCondition.h"
#include "WithinAreaAlertCondition.h"
#include "WithinDistanceAlertCondition.h"

namespace Dsa
{
namespace Alerts
{

const QString AlertConstants::ALERT_CONDITIONS_PROPERTYNAME = "Conditions";
const QString AlertConstants::ATTRIBUTE_NAME = "attribute_name";
const QString AlertConstants::CONDITION_TYPE = "condition_type";
const QString AlertConstants::CONDITION_NAME = "name";
const QString AlertConstants::CONDITION_LEVEL = "level";
const QString AlertConstants::CONDITION_SOURCE = "source";
const QString AlertConstants::CONDITION_QUERY = "query";
const QString AlertConstants::CONDITION_TARGET = "target";
const QString AlertConstants::METERS = "meters";
const QString AlertConstants::MY_LOCATION = "My Location";

QString AlertConstants::attributeEqualsAlertConditionType()
{
  return AttributeEqualsAlertCondition::staticMetaObject.className();
}

QString AlertConstants::withinAreaAlertConditionType()
{
  return WithinAreaAlertCondition::staticMetaObject.className();
}

QString AlertConstants::withinDistanceAlertConditionType()
{
  return WithinDistanceAlertCondition::staticMetaObject.className();
}

} // Alerts
} // Dsa
