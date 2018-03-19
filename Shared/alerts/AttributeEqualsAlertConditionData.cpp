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

#include "pch.hpp"

#include "AlertSource.h"
#include "AlertTarget.h"
#include "AttributeEqualsAlertConditionData.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class AttributeEqualsAlertConditionData
  \inherits AlertConditionData
  \brief Represents the data to be tested as part of a non-spatial, attribute condition.

  This condition data allows a query to determine whether a source object currently matches
  a given query of the form "[my_attribute] = [my_value]".

  The target should be a fixed value whereas the attributes of the source object may change.
 */

/*!
  \brief Constructor for a new attribute equals condition data object.

  \list
    \li \a name. The name of the condition.
    \li \a level. The \l AlertLevel for the condition.
    \li \a source. The source data for the condition (for example
      \l Esri::ArcGISRuntime::Graphic or a location).
    \li \a target. The target data for the condition. This should be a fixed value.
    \li \a attributeName. The name of the attribute in the source object to be queried.
    \li \a parent. The (optional) parent object.
  \endlist
 */
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

/*!
  \brief Destructor.
 */
AttributeEqualsAlertConditionData::~AttributeEqualsAlertConditionData()
{

}

/*!
  \brief Returns whether the source data currently has an attribute value
  matching the target value.
 */
bool AttributeEqualsAlertConditionData::matchesQuery() const
{
  if (!isQueryOutOfDate())
    return cachedQueryResult();

  const QVariant sourceValue = source()->value(attributeName());
  if (sourceValue.isNull() || !sourceValue.isValid())
    return false;

  const QVariant targetValue = target()->targetValue();
  if (targetValue.isNull() || !targetValue.isValid())
    return false;

  return sourceValue == targetValue;
}

/*!
  \brief Returns the name of the attribute to be tested.
 */
QString AttributeEqualsAlertConditionData::attributeName() const
{
    return m_attributeName;
}
