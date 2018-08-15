
/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

#include "AlertConstants.h"

// example app headers
#include "AttributeEqualsAlertCondition.h"
#include "WithinAreaAlertCondition.h"
#include "WithinDistanceAlertCondition.h"

namespace Dsa {

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

} // Dsa
