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
#include "IdsAlertFilter.h"

/*!
  \class IdsAlertFilter
  \inherits AlertFilter
  \brief Represents a filter to be applied to an alert condition data
  in order to exclude specific IDs.
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
IdsAlertFilter::IdsAlertFilter(QObject* parent):
  AlertFilter(parent)
{

}

/*!
  \brief Destructor.
 */
IdsAlertFilter::~IdsAlertFilter()
{

}


/*!
  \brief Returns whether \a conditionData is contained in the list of IDs
  to be excluded.
 */
bool IdsAlertFilter::passesFilter(AlertConditionData* conditionData) const
{
  if (!conditionData)
    return false;

  return !m_ids.contains(conditionData->id());
}

/*!
  \brief Adds \a id to the list of IDs to be excluded.
 */
void IdsAlertFilter::addId(const QUuid& id)
{
  m_ids.insert(id);
}

