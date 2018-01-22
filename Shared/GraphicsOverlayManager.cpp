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

#include "GraphicsOverlayManager.h"

#include "Graphic.h"
#include "GraphicsOverlay.h"

using namespace Esri::ArcGISRuntime;


GraphicsOverlayManager::GraphicsOverlayManager(GraphicsOverlay* overlay, QObject* parent):
  AbstractOverlayManager(parent),
  m_overlay(overlay)
{
  connect(m_overlay->graphics(), &GraphicListModel::graphicAdded, this, &GraphicsOverlayManager::elementAdded);
}

GraphicsOverlayManager::~GraphicsOverlayManager()
{

}

void GraphicsOverlayManager::setSelected(GeoElement* element, bool on)
{
  if (!element)
    return;

  Graphic* g = qobject_cast<Graphic*>(element);
  if (g)
    g->setSelected(on);
}

QString GraphicsOverlayManager::elementDescription(GeoElement* element) const
{
  if (!element)
    return "";

  Graphic* g = qobject_cast<Graphic*>(element);
  if (!g)
    return "";

  GraphicListModel* graphics = m_overlay->graphics();
  if (!graphics)
    return "";

  return QString("%1 (%2)").arg(m_overlay->overlayId(), graphics->indexOf(g));

}

GeoElement* GraphicsOverlayManager::elementAt(int elementId) const
{
  return m_overlay->graphics() ? m_overlay->graphics()->at(elementId) : nullptr;
}

qint64 GraphicsOverlayManager::numberOfElements() const
{
  return m_overlay->graphics()->rowCount();
}
