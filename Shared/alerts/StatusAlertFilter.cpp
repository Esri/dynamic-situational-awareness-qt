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

#include "StatusAlertFilter.h"

// dsa app headers
#include "AlertFilter.h"

namespace Dsa {

/*!
  \class Dsa::StatusAlertFilter
  \inmodule Dsa
  \inherits AlertFilter
  \brief Represents a filter to be applied to an alert condition data
  in order to exclude conditions below a given \l AlertLevel.
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
StatusAlertFilter::StatusAlertFilter(QObject* parent):
  AlertFilter(parent)
{

}

/*!
  \brief Destructor.
 */
StatusAlertFilter::~StatusAlertFilter()
{

}

/*!
  \brief Returns whether \a conditionData has a level which exceeds the minimum
  \l AlertLevel for the filter.
 */
bool StatusAlertFilter::passesFilter(AlertConditionData* connditionData) const
{
  if (!connditionData)
    return false;

  return connditionData->level() >= m_minLevel;
}

/*!
  \brief The minimum \l AlertLevel for the filter.

  The default is \c Low.
 */
AlertLevel StatusAlertFilter::minLevel() const
{
  return m_minLevel;
}


/*!
  \brief Sets he minimum \l AlertLevel for the filter to \a minLevel.
 */
void StatusAlertFilter::setMinLevel(AlertLevel minLevel)
{
  if (minLevel == m_minLevel)
    return;

  m_minLevel = minLevel;
  emit filterChanged();
}


} // Dsa
