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

#ifndef ALERTCONSTANTS_H
#define ALERTCONSTANTS_H

// Qt headers
#include <QString>

namespace Dsa {

class AlertConstants {
public:
  static const QString ALERT_CONDITIONS_PROPERTYNAME;
  static const QString ATTRIBUTE_NAME;
  static const QString CONDITION_TYPE;
  static const QString CONDITION_NAME;
  static const QString CONDITION_LEVEL;
  static const QString CONDITION_SOURCE;
  static const QString CONDITION_QUERY;
  static const QString CONDITION_TARGET;
  static const QString METERS;
  static const QString MY_LOCATION;

  static QString attributeEqualsAlertConditionType();
  static QString withinAreaAlertConditionType();
  static QString withinDistanceAlertConditionType();
};

} // Dsa

#endif // ALERTCONSTANTS_H
