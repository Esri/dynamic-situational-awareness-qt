
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

#include "AlertFilter.h"

namespace Dsa {

/*!
  \class Dsa::AlertFilter
  \inmodule Dsa
  \inherits QObject
  \brief Represents a filter to be applied to an alert condition data.

  This allows for certain condition data objects to be excluded - for
  example by \l AlertLevel or by specific ID.
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
AlertFilter::AlertFilter(QObject* parent):
  QObject(parent)
{
}

/*!
  \brief Destructor.
 */
AlertFilter::~AlertFilter()
{
}

} // Dsa

// Signal Documentation
/*!
  \fn void AlertFilter::filterChanged();
  \brief Signal emitted when the filter changes.
 */
