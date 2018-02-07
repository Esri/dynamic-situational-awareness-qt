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

#ifndef ALERTCONSTANTS_H
#define ALERTCONSTANTS_H

#include <QString>

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

#endif // ALERTCONSTANTS_H
