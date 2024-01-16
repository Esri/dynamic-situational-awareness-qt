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
  \inherits QObject
  \brief Manages a \l Esri::ArcGISRuntime::GraphicsOverlay object
  for displaying message feed graphics.

  The overlay currently only supports messages containing a
  point geometry type.
 */

/*!
  \brief Constructor taking a \a geoView and an optional \a parent.
 */

/*!
  \brief Constructor taking a \a geoView, a \a renderer, a \a messageType,
  a \a surfacePlacement mode and an optional \a parent.
 */

/*!
  \brief Destructor.
 */
MessagesOverlay::MessagesOverlay(GeoView* geoView, Renderer* renderer, const QString& messageType, MessageFeed* messageFeed, SurfacePlacement surfacePlacement, QObject* parent) :
  QObject(parent),
  m_geoView(geoView),
  m_renderer(renderer),
  m_surfacePlacement(surfacePlacement),
  m_messageFeed(messageFeed)
{
  m_dynamicEntityLayer = new DynamicEntityLayer(m_messageFeed, this);
  m_dynamicEntityLayer->setLayerId(messageType);
  m_dynamicEntityLayer->setSceneProperties(LayerSceneProperties(m_surfacePlacement));
  m_dynamicEntityLayer->setRenderer(m_renderer);
  m_dynamicEntityLayer->setName(messageType);
  SceneView* scene = static_cast<SceneView*>(m_geoView);
  scene->arcGISScene()->operationalLayers()->append(m_dynamicEntityLayer);
}

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

  m_dynamicEntityLayer->setRenderer(m_renderer);
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

  m_dynamicEntityLayer->setSceneProperties(LayerSceneProperties(m_surfacePlacement));
}

/*!
  \brief Returns the message type for the overlay.
 */
QString MessagesOverlay::messageType() const
{
  return m_dynamicEntityLayer->layerId();
}

/*!
  \brief Sets the message type for the overlay to \a messageType.
 */
void MessagesOverlay::setMessageType(const QString& messageType)
{
  if (m_dynamicEntityLayer->layerId() == messageType)
    return;

  m_dynamicEntityLayer->setLayerId(messageType);
}

/*!
  \brief Returns the Esri:ArcGISRuntime::GraphicsOverlay object for the overlay.
 */
DynamicEntityLayer* MessagesOverlay::dynamicEntityLayer() const
{
  return m_dynamicEntityLayer;
}

/*!
  \brief Returns the Esri:ArcGISRuntime::GeoView for the overlay.
 */
GeoView* MessagesOverlay::geoView() const
{
  return m_geoView;
}

/*!
  \brief Returns whether the overlay is visible.
 */
bool MessagesOverlay::isVisible() const
{
  return m_dynamicEntityLayer->isVisible();
}

/*!
  \brief Sets the overlay to be \a visible.
 */
void MessagesOverlay::setVisible(bool visible)
{
  if (m_dynamicEntityLayer->isVisible() == visible)
    return;

  m_dynamicEntityLayer->setVisible(visible);

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
