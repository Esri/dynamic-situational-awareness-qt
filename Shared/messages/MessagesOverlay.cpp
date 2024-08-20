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
#include "LayerListModel.h"
#include "pch.hpp"

#include "MessagesOverlay.h"

// DSA app headers
#include "MessageFeed.h"

// C++ API headers
#include "GeoView.h"
#include "LayerSceneProperties.h"
#include "Renderer.h"
#include "SceneViewTypes.h"
#include "DynamicEntityLayer.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::MessagesOverlay
  \inmodule Dsa
  \inherits DynamicEntityLayer
  \brief Manages a \l Esri::ArcGISRuntime::DynamicEntityLayer object
  for displaying message feed graphics.

  The overlay currently only supports messages containing a
  point geometry type.
 */

/*!
  \brief Constructor taking a \a messageFeed, a \a messageType
  and an optional \a parent.
 */
MessagesOverlay::MessagesOverlay(MessageFeed* messageFeed, const QString& messageType, QObject* parent) :
    DynamicEntityLayer(messageFeed, parent),
    m_messageFeed(messageFeed)
{
  this->setLayerId(messageType);
  this->setName(messageType);
  messageFeed->setMessagesOverlay(this);
}

/*!
  \brief Destructor.
 */
MessagesOverlay::~MessagesOverlay() = default;

/*!
  \brief Returns the message type for the overlay.
 */
QString MessagesOverlay::messageType() const
{
  return this->layerId();
}

/*!
  \brief Sets the message type for the overlay to \a messageType.
 */
void MessagesOverlay::setMessageType(const QString& messageType)
{
  if (this->layerId() == messageType)
    return;

  this->setLayerId(messageType);
}

DynamicEntity* MessagesOverlay::getDynamicEntityById(quint64 entityId)
{
  return m_messageFeed->getDynamicEntityById(entityId);
}

const QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*>& MessagesOverlay::dynamicEntities()
{
  return m_messageFeed->dynamicEntities();
}

} // Dsa
