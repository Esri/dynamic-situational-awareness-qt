
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

#include "AlertSource.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::AlertSource
  \inmodule Dsa
  \inherits QObject
  \brief Represents a source (generally a real-time feed) for an \l AlertCondition.

  \note This is an abstract base type.
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
AlertSource::AlertSource(QObject* parent):
  QObject(parent)
{
}

/*!
  \brief Destructor.
 */
AlertSource::~AlertSource()
{
  emit noLongerValid();
}

} // Dsa

// Signal Documentation
/*!
  \fn void AlertSource::dataChanged();
  \brief Signal emitted when alert source's data changes.
 */

/*!
  \fn void AlertSource::noLongerValid();
  \brief Signal emitted when the alert source is no longer valid.
 */
