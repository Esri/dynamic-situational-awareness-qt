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

#include "GraphicsOverlayAlertTarget.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

GraphicsOverlayAlertTarget::GraphicsOverlayAlertTarget(GraphicsOverlay* graphicsOverlay):
  AlertTarget(graphicsOverlay),
  m_graphicsOverlay(graphicsOverlay)
{
  connect(m_graphicsOverlay->graphics(), &GraphicListModel::graphicRemoved, this, [this](int index)
  {
    auto conn = m_graphicConnections.at(index);
    disconnect(conn);
    m_graphicConnections.removeAt(index);
    emit locationChanged();
  });

  connect(m_graphicsOverlay->graphics(), &GraphicListModel::graphicAdded, this, [this](int index)
  {
    Graphic* graphic = m_graphicsOverlay->graphics()->at(index);
    setupGraphicConnections(graphic);
  });

  const GraphicListModel* graphics = m_graphicsOverlay->graphics();
  if (!graphics)
    return;

  const int count = graphics->rowCount();
  for (int i = 0; i < count; ++i)
    setupGraphicConnections(m_graphicsOverlay->graphics()->at(i));
}

GraphicsOverlayAlertTarget::~GraphicsOverlayAlertTarget()
{

}

QList<Geometry> GraphicsOverlayAlertTarget::location() const
{
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

