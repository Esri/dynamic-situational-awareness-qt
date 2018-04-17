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

#include "StatusAlertFilter.h"

// example app headers
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
  \brief Sets he minimum \l AlertLevel for the filter to \l minLevel.
 */
void StatusAlertFilter::setMinLevel(AlertLevel minLevel)
{
  if (minLevel == m_minLevel)
    return;

  m_minLevel = minLevel;
  emit filterChanged();
}


} // Dsa
