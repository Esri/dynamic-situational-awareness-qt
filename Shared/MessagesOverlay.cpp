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
#include "DictionarySymbolStyle.h"
#include "GraphicsOverlay.h"
#include "Message.h"
#include "DictionaryRenderer.h"

using namespace Esri::ArcGISRuntime;

MessagesOverlay::MessagesOverlay(GeoView* geoView, QObject* parent) :
  MessagesOverlay(geoView, nullptr, parent)
{
}

MessagesOverlay::MessagesOverlay(GeoView* geoView, DictionarySymbolStyle* dictionarySymbolStyle, QObject* parent) :
  QObject(parent),
  m_geoView(geoView),
  m_dictionarySymbolStyle(dictionarySymbolStyle)
{
}

MessagesOverlay::~MessagesOverlay()
{
}

DictionarySymbolStyle* MessagesOverlay::dictionarySymbolStyle() const
{
  return m_dictionarySymbolStyle.data();
}

void MessagesOverlay::setDictionarySymbolStyle(DictionarySymbolStyle* dictionarySymbolStyle)
{
  m_dictionarySymbolStyle = dictionarySymbolStyle;
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
  const auto symbolId = message.symbolId();
  const auto geometry = message.geometry();

  if (geometry.isEmpty() || messageId.isEmpty() ||
      symbolId.isEmpty())
    return false;

  if (m_existingGraphics.contains(messageId))
  {
    // update existing graphic attributes and geometry
    // if the graphic already exists in the hash
    Graphic* graphic = m_existingGraphics[messageId];
    if (graphic->geometry().geometryType() != geometry.geometryType())
      return false;

    graphic->setGeometry(geometry);
    graphic->attributes()->setAttributesMap(message.attributes());
    return true;
  }

  // add new graphic
  std::unique_ptr<Graphic> graphic(new Graphic(geometry, message.attributes(), this));
  Graphic* graphicPtr = nullptr;

  switch (geometry.geometryType())
  {
  case GeometryType::Point:
  case GeometryType::Multipoint:
    if (!m_pointGraphicsOverlay)
    {
      if (!m_dictionarySymbolStyle || !m_geoView)
      {
        return false;
      }

      // add point/multipoint geometry types to a dynamically-rendered graphics overlay
      m_pointGraphicsOverlay = new GraphicsOverlay(this);
      m_pointGraphicsOverlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
      m_pointGraphicsOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
      m_pointGraphicsOverlay->setRenderer(new DictionaryRenderer(m_dictionarySymbolStyle.data(), this));
      m_graphicsOverlays.append(m_pointGraphicsOverlay);
      m_geoView->graphicsOverlays()->append(m_pointGraphicsOverlay);

      emit graphicsOverlaysChanged();
    }

    graphicPtr = graphic.release();
    m_pointGraphicsOverlay->graphics()->append(graphicPtr);
    break;
  case GeometryType::Envelope:
  case GeometryType::Polygon:
  case GeometryType::Polyline:
    if (!m_linePolygonGraphicsOverlay)
    {
      if (!m_dictionarySymbolStyle || !m_geoView)
      {
        return false;
      }

      // add polygon/polyline geometry types to a statically-rendered graphics overlay
      m_linePolygonGraphicsOverlay = new GraphicsOverlay(this);
      m_linePolygonGraphicsOverlay->setRenderingMode(GraphicsRenderingMode::Static);
      m_linePolygonGraphicsOverlay->setRenderer(new DictionaryRenderer(m_dictionarySymbolStyle.data(), this));
      m_graphicsOverlays.append(m_linePolygonGraphicsOverlay);
      m_geoView->graphicsOverlays()->append(m_linePolygonGraphicsOverlay);

      emit graphicsOverlaysChanged();
    }

    graphicPtr = graphic.release();
    m_linePolygonGraphicsOverlay->graphics()->append(graphicPtr);
    break;
  default:
    return false;
  }

  if (graphicPtr)
    m_existingGraphics.insert(messageId, graphicPtr);

  return true;
}
