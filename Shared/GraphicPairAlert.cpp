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

#include "GraphicPairAlert.h"

#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

GraphicPairAlert::GraphicPairAlert(Graphic* graphic1, Graphic* graphic2, double distance, QObject* parent):
  ProximityPairAlert(graphic1, graphic2, distance, parent),
  m_graphic1(graphic1),
  m_graphic2(graphic2),
  m_description1("Graphic1"),
  m_description2("Graphic2")
{
  if (m_graphic1->graphicsOverlay())
  {
    GraphicListModel* graphics1 = m_graphic1->graphicsOverlay()->graphics();
    if (graphics1)
      m_description1 = QString("%1 (%2)").arg(m_graphic1->graphicsOverlay()->overlayId(), graphics1->indexOf(m_graphic1));
  }

  if (m_graphic2->graphicsOverlay())
  {
    GraphicListModel* graphics2 = m_graphic2->graphicsOverlay()->graphics();
    if (graphics2)
      m_description2 = QString("%1 (%2)").arg(m_graphic2->graphicsOverlay()->overlayId(), graphics2->indexOf(m_graphic2));
  }
}

GraphicPairAlert::~GraphicPairAlert()
{

}

void GraphicPairAlert::highlight(bool on)
{
  if (m_graphic1)
    m_graphic1->setSelected(on);
}

QString GraphicPairAlert::element1Description() const
{
  return m_description1;
}

QString GraphicPairAlert::element2Description() const
{
  return m_description2;
}
