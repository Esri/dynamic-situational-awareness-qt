
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

#include "IdsAlertFilter.h"

// example app headers
#include "AlertConditionData.h"

namespace Dsa {

/*!
  \class Dsa::IdsAlertFilter
  \inmodule Dsa
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
