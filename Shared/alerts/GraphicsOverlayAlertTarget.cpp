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

/*!
  \class GraphicsOverlayAlertTarget
  \inherits AlertSpatialTarget
  \brief Represents a target based on an \l Esri::ArcGISRuntime::GraphicsOverlay
  for an \l AlertCondition.

  Changes to any of the graphics in the overlay will cause the \l AlertTarget::locationChanged
  signal to be emitted.
  */

/*!
  \brief Constructor taking an \l Esri::ArcGISRuntime::GraphicsOverlay (\a graphicsOverlay).

  All graphics will be retrieved from the underlying overlay and any new graphics which are added
  will also be handled.
 */
GraphicsOverlayAlertTarget::GraphicsOverlayAlertTarget(GraphicsOverlay* graphicsOverlay):
  AlertSpatialTarget(graphicsOverlay),
  m_graphicsOverlay(graphicsOverlay)
{
  // respond to graphics being removed from the overlay
  connect(m_graphicsOverlay->graphics(), &GraphicListModel::graphicRemoved, this, [this](int)
  {
    rebuildQuadtree();
    emit dataChanged();
  });

  // respond to graphics being added to the overlay
  connect(m_graphicsOverlay->graphics(), &GraphicListModel::graphicAdded, this, [this](int index)
  {
    Graphic* graphic = m_graphicsOverlay->graphics()->at(index);
    setupGraphicConnections(graphic);
    if (m_quadtree)
      m_quadtree->appendGeoElment(graphic);
    else
      rebuildQuadtree();

    emit dataChanged();
  });

  // build the quadtree for all graphics in the overlay to begin with
  rebuildQuadtree();
}

/*!
  \brief Destructor.
 */
GraphicsOverlayAlertTarget::~GraphicsOverlayAlertTarget()
{

}

/*!
  \brief Returns the list of \l Esri::ArcGISRuntime::Geometry which are in the \a targetArea.

  \note No exact intersection tests are carried out to create this list.
 */
QList<Geometry> GraphicsOverlayAlertTarget::targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const
{
  // if the quadtree has been built, use  it to return the set of candidate geometries
  if (m_quadtree)
    return m_quadtree->candidateIntersections(targetArea);

  // otherwise, return all of the geometry in the overlay
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

/*!
  \internal

  Connect signals etc. for a new \a graphic.
 */
void GraphicsOverlayAlertTarget::setupGraphicConnections(Graphic* graphic)
{
  if (!graphic)
    return;

  m_graphicConnections.append(connect(graphic, &Graphic::geometryChanged, this, &GraphicsOverlayAlertTarget::dataChanged));
}

/*!
  \internal

  Build the quadtree.
 */
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
    elements.append(g);
  }

  // if there is more than 1 element in the overlay, build a quadtree
  if (elements.size() > 1)
    m_quadtree = new GeometryQuadtree(m_graphicsOverlay->extent(), elements, 8, this);
}

