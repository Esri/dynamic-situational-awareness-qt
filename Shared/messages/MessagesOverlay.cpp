
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

#include "MessagesOverlay.h"

// example app headers
#include "Message.h"

// C++ API headers
#include "GeoView.h"
#include "GraphicsOverlay.h"
#include "Renderer.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::MessagesOverlay
  \inmodule Dsa
  \inherits QObject
  \brief Manages a \l Esri::ArcGISRuntime::GraphicsOverlay object
  for displaying message feed graphics.

  The overlay currently only supports messages containing a
  point geometry type.
 */

/*!
  \brief Constructor taking a \a geoView and an optional \a parent.
 */
MessagesOverlay::MessagesOverlay(GeoView* geoView, QObject* parent) :
  MessagesOverlay(geoView, nullptr, QString(), SurfacePlacement::Draped, parent)
{
}

/*!
  \brief Constructor taking a \a geoView, a \a renderer, a \a messageType,
  a \a surfacePlacement mode and an optional \a parent.
 */
MessagesOverlay::MessagesOverlay(GeoView* geoView, Renderer* renderer, const QString& messageType,
                                 SurfacePlacement surfacePlacement, QObject* parent) :
  QObject(parent),
  m_geoView(geoView),
  m_renderer(renderer),
  m_surfacePlacement(surfacePlacement),
  m_graphicsOverlay(new GraphicsOverlay(this))
{
  m_graphicsOverlay->setOverlayId(messageType);
  m_graphicsOverlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
  m_graphicsOverlay->setSceneProperties(LayerSceneProperties(m_surfacePlacement));
  m_graphicsOverlay->setRenderer(m_renderer);
  m_graphicsOverlay->setSelectionColor(Qt::red);
  m_geoView->graphicsOverlays()->append(m_graphicsOverlay);
}

/*!
  \brief Destructor.
 */
MessagesOverlay::~MessagesOverlay()
{
}

/*!
  \brief Returns the Esri:ArcGISRuntime::Renderer in use by the overlay.
 */
Renderer* MessagesOverlay::renderer() const
{
  return m_renderer.data();
}

/*!
  \brief Sets the Esri:ArcGISRuntime::Renderer for the overlay to \a renderer.
 */
void MessagesOverlay::setRenderer(Renderer* renderer)
{
  if (m_renderer == renderer)
    return;

  m_renderer = renderer;

  m_graphicsOverlay->setRenderer(m_renderer);
}

/*!
  \brief Returns the Esri:ArcGISRuntime::SurfacePlacement for the overlay.
 */
SurfacePlacement MessagesOverlay::surfacePlacement() const
{
  return m_surfacePlacement;
}

/*!
  \brief Sets the Esri:ArcGISRuntime::SurfacePlacement for the overlay to \a surfacePlacement.
 */
void MessagesOverlay::setSurfacePlacement(SurfacePlacement surfacePlacement)
{
  if (m_surfacePlacement == surfacePlacement)
    return;

  m_surfacePlacement = surfacePlacement;

  m_graphicsOverlay->setSceneProperties(LayerSceneProperties(m_surfacePlacement));
}

/*!
  \brief Returns the message type for the overlay.
 */
QString MessagesOverlay::messageType() const
{
  return m_graphicsOverlay->overlayId();
}

/*!
  \brief Sets the message type for the overlay to \a messageType.
 */
void MessagesOverlay::setMessageType(const QString& messageType)
{
  if (m_graphicsOverlay->overlayId() == messageType)
    return;

  m_graphicsOverlay->setOverlayId(messageType);
}

/*!
  \brief Returns the Esri:ArcGISRuntime::GraphicsOverlay object for the overlay.
 */
GraphicsOverlay* MessagesOverlay::graphicsOverlay() const
{
  return m_graphicsOverlay;
}

/*!
  \brief Returns the Esri:ArcGISRuntime::GeoView for the overlay.
 */
GeoView* MessagesOverlay::geoView() const
{
  return m_geoView;
}

/*!
  \brief Adds the \l Message \a message to the overlay. Returns whether adding was successful.
 */
bool MessagesOverlay::addMessage(const Message& message)
{
  const auto messageId = message.messageId();
  if (messageId.isEmpty())
  {
    emit errorOccurred(QStringLiteral("Failed to add message - message ID is empty"));
    return false;
  }

  if (message.messageType() != messageType())
  {
    emit errorOccurred(QStringLiteral("Failed to add message - message type mismatch"));
    return false;
  }

  const auto symbolId = message.symbolId();
  const auto geometry = message.geometry();
  const auto messageAction = message.messageAction();

  if (messageAction == Message::MessageAction::Update)
  {
    if (m_renderer && m_renderer->rendererType() == RendererType::DictionaryRenderer && symbolId.isEmpty())
    {
      emit errorOccurred(QStringLiteral("Failed to add message - symbol ID is empty"));
      return false;
    }

    if (geometry.isEmpty())
    {
      emit errorOccurred(QStringLiteral("Failed to add message - geometry is empty"));
      return false;
    }

    if (geometry.geometryType() != GeometryType::Point)
    {
      emit errorOccurred(QStringLiteral("Failed to add message - only point geometry types are supported"));
      return false;
    }
  }

  if (m_existingGraphics.contains(messageId))
  {
    // update existing graphic attributes and geometry
    // if the graphic already exists in the hash
    Graphic* graphic = m_existingGraphics[messageId];

    switch (messageAction)
    {
    case Message::MessageAction::Update:
    case Message::MessageAction::Select:
    case Message::MessageAction::Unselect:
    {
      const Geometry geom = graphic->geometry();
      if (geom.geometryType() != geometry.geometryType())
        return false;

      if (!(geom == geometry))
        graphic->setGeometry(geometry);

      graphic->attributes()->setAttributesMap(message.attributes());

      if (messageAction == Message::MessageAction::Select)
      {
        graphic->setSelected(true);
      }
      else if (messageAction == Message::MessageAction::Unselect)
      {
        graphic->setSelected(false);
      }

      break;
    }
    case Message::MessageAction::Remove:
    {
      m_graphicsOverlay->graphics()->removeOne(graphic);
      break;
    }
    default:
      emit errorOccurred(QStringLiteral("Unknown message action"));
      return false;
    }

    return true;
  }

  if (messageAction != Message::MessageAction::Update)
  {
    emit errorOccurred(QStringLiteral("Message action must be Update to add new message"));
    return false;
  }

  // add new graphic
  Graphic* graphic = new Graphic(geometry, message.attributes(), this);
  m_graphicsOverlay->graphics()->append(graphic);
  m_existingGraphics.insert(messageId, graphic);

  return true;
}

/*!
  \brief Returns whether the overlay is visible.
 */
bool MessagesOverlay::isVisible() const
{
  return m_graphicsOverlay->isVisible();
}

/*!
  \brief Sets the overlay to be \a visible.
 */
void MessagesOverlay::setVisible(bool visible)
{
  if (m_graphicsOverlay->isVisible() == visible)
    return;

  m_graphicsOverlay->setVisible(visible);

  emit visibleChanged();
}

} // Dsa

// Signal Documentation
/*!
  \fn void MessagesOverlay::visibleChanged();
  \brief Signal emitted when the visibility of the overlay changes.
 */

/*!
  \fn void MessagesOverlay::errorOccurred(const QString& error);
  \brief Signal emitted when an \a error occurs.
 */
