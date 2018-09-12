
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

#include "AlertTarget.h"

namespace Dsa {

/*!
  \class Dsa::AlertTarget
  \inmodule Dsa
  \inherits QObject
  \brief Represents a target (e.g. an overlay or a real-time feed) for an \l AlertCondition.

  \note This is an abstract base type.
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
AlertTarget::AlertTarget(QObject* parent):
  QObject(parent)
{
}

/*!
  \brief Destructor.
 */
AlertTarget::~AlertTarget()
{
  emit noLongerValid();
}

} // Dsa

// Signal Documentation
/*!
  \fn void AlertTarget::noLongerValid();
  \brief Signal emitted when alert target is no longer valid.
 */

/*!
  \fn void AlertTarget::dataChanged();
  \brief Signal emitted when alert target's data changes.
 */

