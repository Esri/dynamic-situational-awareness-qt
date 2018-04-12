// Copyright 2018 ESRI
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

// PCH header
#include "pch.hpp"

#include "IdsAlertFilter.h"

// example app headers
#include "AlertConditionData.h"

namespace Dsa {

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
  \brief Returns whether \a conditionData is not contained in the list of IDs
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
  if (m_ids.contains(id))
      return;

  m_ids.insert(id);
  emit filterChanged();
}

/*!
  \brief Removes all IDs from the filter.
 */
void IdsAlertFilter::clearIds()
{
  if (m_ids.isEmpty())
    return;

  m_ids.clear();
  emit filterChanged();
}

} // Dsa
