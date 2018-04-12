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
  \class MessagesOverlay
  \inherits QObject
  \brief Manages a set of \l Esri::ArcGISRuntime::GraphicsOverlay objects
  for displaying message a feed.
 */

/*!
  \brief Constructor taking a \a geoView and an optional \a parent.
 */
MessagesOverlay::MessagesOverlay(GeoView* geoView, QObject* parent) :
  MessagesOverlay(geoView, nullptr, SurfacePlacement::Draped, parent)
{
}

/*!
  \brief Constructor taking a \a geoView, a \a renderer, a \a surfacePlacement mode and an optional \a parent.
 */
MessagesOverlay::MessagesOverlay(GeoView* geoView, Renderer* renderer, SurfacePlacement surfacePlacement, QObject* parent) :
  QObject(parent),
  m_geoView(geoView),
  m_renderer(renderer),
  m_surfacePlacement(surfacePlacement)
{
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
  m_renderer = renderer;

  for (GraphicsOverlay* go : m_graphicsOverlays)
  {
    go->setRenderer(m_renderer);
  }
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
  m_surfacePlacement = surfacePlacement;

  if (m_pointGraphicsOverlay)
  {
    m_pointGraphicsOverlay->setSceneProperties(LayerSceneProperties(m_surfacePlacement));
  }
}

/*!
  \brief Returns the list of Esri:ArcGISRuntime::GraphicsOverlay objects for the overlay.
 */
QList<GraphicsOverlay*> MessagesOverlay::graphicsOverlays() const
{
  return m_graphicsOverlays;
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
      switch (geometry.geometryType())
      {
        case GeometryType::Point:
        case GeometryType::Multipoint:
        {
          if (m_pointGraphicsOverlay)
          {
            m_pointGraphicsOverlay->graphics()->removeOne(graphic);
          }
          break;
        }
        case GeometryType::Envelope:
        case GeometryType::Polygon:
        case GeometryType::Polyline:
        {
          if (m_linePolygonGraphicsOverlay)
          {
            m_linePolygonGraphicsOverlay->graphics()->removeOne(graphic);
          }
          break;
        }
      default:
          break;
      }

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

  switch (geometry.geometryType())
  {
  case GeometryType::Point:
  case GeometryType::Multipoint:
    if (!m_pointGraphicsOverlay)
    {
      if (!m_renderer || !m_geoView)
      {
        delete graphic;
        return false;
      }

      // add point/multipoint geometry types to a dynamically-rendered graphics overlay
      m_pointGraphicsOverlay = new GraphicsOverlay(this);
      m_pointGraphicsOverlay->setOverlayId(message.messageType());
      m_pointGraphicsOverlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
      m_pointGraphicsOverlay->setSceneProperties(LayerSceneProperties(m_surfacePlacement));
      m_pointGraphicsOverlay->setRenderer(m_renderer);
      m_pointGraphicsOverlay->setSelectionColor(Qt::red);
      m_graphicsOverlays.append(m_pointGraphicsOverlay);
      m_geoView->graphicsOverlays()->append(m_pointGraphicsOverlay);

      emit graphicsOverlaysChanged();
      emit visibleChanged();
    }

    m_pointGraphicsOverlay->graphics()->append(graphic);
    break;
  case GeometryType::Envelope:
  case GeometryType::Polygon:
  case GeometryType::Polyline:
    if (!m_linePolygonGraphicsOverlay)
    {
      if (!m_renderer || !m_geoView)
      {
        delete graphic;
        return false;
      }

      // add polygon/polyline geometry types to a statically-rendered graphics overlay
      m_linePolygonGraphicsOverlay = new GraphicsOverlay(this);
      m_linePolygonGraphicsOverlay->setRenderingMode(GraphicsRenderingMode::Static);
      m_linePolygonGraphicsOverlay->setRenderer(m_renderer);
      m_graphicsOverlays.append(m_linePolygonGraphicsOverlay);
      m_geoView->graphicsOverlays()->append(m_linePolygonGraphicsOverlay);

      emit graphicsOverlaysChanged();
      emit visibleChanged();
    }

    m_linePolygonGraphicsOverlay->graphics()->append(graphic);
    break;
  default:
    delete graphic;
    return false;
  }

  m_existingGraphics.insert(messageId, graphic);

  return true;
}

/*!
  \brief Returns whether the overlay is visible.
 */
bool MessagesOverlay::isVisible() const
{
  // at this time, only one graphics overlay per message overlay
  // this could change later...
  if (m_pointGraphicsOverlay)
    return m_pointGraphicsOverlay->isVisible();
  else if (m_linePolygonGraphicsOverlay)
    return m_linePolygonGraphicsOverlay->isVisible();

  return false;
}

/*!
  \brief Sets the overlay to be \a visible.
 */
void MessagesOverlay::setVisible(bool visible)
{
  if (m_pointGraphicsOverlay)
    m_pointGraphicsOverlay->setVisible(visible);

  if (m_linePolygonGraphicsOverlay)
    m_linePolygonGraphicsOverlay->setVisible(visible);
}

} // Dsa
