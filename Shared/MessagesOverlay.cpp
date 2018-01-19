// Copyright 2017 ESRI
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

#include "MessagesOverlay.h"
#include "GeoView.h"
#include "Renderer.h"
#include "GraphicsOverlay.h"
#include "Message.h"

using namespace Esri::ArcGISRuntime;

MessagesOverlay::MessagesOverlay(GeoView* geoView, QObject* parent) :
  MessagesOverlay(geoView, nullptr, parent)
{
}

MessagesOverlay::MessagesOverlay(GeoView* geoView, Renderer* renderer, QObject* parent) :
  QObject(parent),
  m_geoView(geoView),
  m_renderer(renderer)
{
}

MessagesOverlay::~MessagesOverlay()
{
}

Renderer* MessagesOverlay::renderer() const
{
  return m_renderer.data();
}

void MessagesOverlay::setRenderer(Renderer* renderer)
{
  m_renderer = renderer;

  for (GraphicsOverlay* go : m_graphicsOverlays)
  {
    go->setRenderer(m_renderer);
  }
}

QList<GraphicsOverlay*> MessagesOverlay::graphicsOverlays() const
{
  return m_graphicsOverlays;
}

GeoView* MessagesOverlay::geoView() const
{
  return m_geoView;
}

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

  if (message.messageAction() == Message::MessageAction::Update)
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

    if (message.messageAction() == Message::MessageAction::Update)
    {
      if (graphic->geometry().geometryType() != geometry.geometryType())
        return false;

      graphic->setGeometry(geometry);
      graphic->attributes()->setAttributesMap(message.attributes());
    }
    else if (message.messageAction() == Message::MessageAction::Remove)
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
      }
    }
    else if (message.messageAction() == Message::MessageAction::Select)
    {
      graphic->setSelected(true);
    }
    else if (message.messageAction() == Message::MessageAction::Unselect)
    {
      graphic->setSelected(false);
    }
    else
    {
      emit errorOccurred(QStringLiteral("Unknown message action"));
      return false;
    }

    return true;
  }

  if (message.messageAction() != Message::MessageAction::Update)
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
      m_pointGraphicsOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
      m_pointGraphicsOverlay->setRenderer(m_renderer);
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

void MessagesOverlay::setVisible(bool visible)
{
  if (m_pointGraphicsOverlay)
    m_pointGraphicsOverlay->setVisible(visible);

  if (m_linePolygonGraphicsOverlay)
    m_linePolygonGraphicsOverlay->setVisible(visible);
}
