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

#include "GeometryQuadtree.h"
#include "GraphicsOverlayAlertTarget.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

GraphicsOverlayAlertTarget::GraphicsOverlayAlertTarget(GraphicsOverlay* graphicsOverlay):
  AlertTarget(graphicsOverlay),
  m_graphicsOverlay(graphicsOverlay)
{
  connect(m_graphicsOverlay->graphics(), &GraphicListModel::graphicRemoved, this, [this](int)
  {
    rebuildQuadtree();
    emit locationChanged();
  });

  connect(m_graphicsOverlay->graphics(), &GraphicListModel::graphicAdded, this, [this](int index)
  {
    Graphic* graphic = m_graphicsOverlay->graphics()->at(index);
    setupGraphicConnections(graphic);
    if (m_quadtree)
      m_quadtree->appendGeoElment(graphic);

    emit locationChanged();
  });

  rebuildQuadtree();
}

GraphicsOverlayAlertTarget::~GraphicsOverlayAlertTarget()
{

}

QList<Geometry> GraphicsOverlayAlertTarget::targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const
{
  if (m_quadtree)
    return m_quadtree->candidateIntersections(targetArea);

  QList<Geometry> geomList;
  const GraphicListModel* graphics = m_graphicsOverlay->graphics();
  if (!graphics)
    return geomList;

  const int count = graphics->rowCount();
  for (int i = 0; i < count; ++i)
  {
    Graphic* graphic = m_graphicsOverlay->graphics()->at(i);
    if (graphic)
      geomList.append(graphic->geometry());
  }

  return geomList;
}

void GraphicsOverlayAlertTarget::setupGraphicConnections(Graphic* graphic)
{
  if (!graphic)
    return;

  m_graphicConnections.append(connect(graphic, &Graphic::geometryChanged, this, &GraphicsOverlayAlertTarget::locationChanged));
}

void GraphicsOverlayAlertTarget::rebuildQuadtree()
{
  if (m_quadtree)
  {
    delete m_quadtree;
    m_quadtree = nullptr;
  }

  const GraphicListModel* graphics = m_graphicsOverlay->graphics();
  if (!graphics)
    return;

  const int count = graphics->rowCount();
  QList<GeoElement*> elements;
  for (int i = 0; i < count; ++i)
  {
    Graphic* g = m_graphicsOverlay->graphics()->at(i);
    if (!g)
      continue;

    setupGraphicConnections(g);
  }

  if (elements.size() > 1)
    m_quadtree = new GeometryQuadtree(m_graphicsOverlay->extent(), elements, 8, this);
}

