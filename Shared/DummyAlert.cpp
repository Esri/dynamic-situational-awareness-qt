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

#include "DummyAlert.h"

#include "Envelope.h"

#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "Point.h"
#include "Symbol.h"
#include "SimpleMarkerSceneSymbol.h"

using namespace Esri::ArcGISRuntime;

DummyAlert::DummyAlert(Graphic* graphic, GraphicsOverlay* overlay, QObject* parent):
  AbstractAlert(parent),
  m_graphic(graphic),
  m_overlay(overlay)
{
  m_graphic->setSelected(true);
}

DummyAlert::~DummyAlert()
{

}

Geometry DummyAlert::position() const
{
  return m_graphic ? m_graphic->geometry() : Point();
}

void DummyAlert::highlight(bool on)
{
  if (!m_graphic)
    return;

  // option 1: select/unselect
//    if (m_graphic)
//      m_graphic->setSelected(on);

  // option 2: change symbol size
  //  Symbol* symbol = m_graphic->symbol();
  //  if (!symbol)
  //    return;

  //  SimpleMarkerSceneSymbol* sms = qobject_cast<SimpleMarkerSceneSymbol*>(symbol);
  //  if (!sms)
  //  return;

  //  constexpr int maxWidth = 50;
  //  const int currWidth = sms->width();
  //  if (m_expanding)
  //  {
  //    if (currWidth <= maxWidth)
  //    {
  //      sms->setWidth(currWidth + 1);
  //      sms->setHeight(currWidth + 1);
  //      sms->setDepth(currWidth + 1);
  //    }
  //    else
  //    {
  //      m_expanding = false;
  //      sms->setWidth(currWidth -1);
  //      sms->setHeight(currWidth -1);
  //      sms->setDepth(currWidth -1);
  //    }
  //  }
  //  else
  //  {
  //    if (currWidth > 25)
  //    {
  //      sms->setWidth(currWidth - 1);
  //      sms->setHeight(currWidth -1);
  //      sms->setDepth(currWidth -1);
  //    }
  //    else
  //    {
  //      m_expanding = true;
  //      sms->setWidth(currWidth + 1);
  //      sms->setHeight(currWidth + 1);
  //      sms->setDepth(currWidth + 1);
  //    }
  //  }

  // option 3 changing geometry
//  Point geom = m_graphic->geometry();
//  constexpr int maxHeight = 30;
//  const int currZ = geom.z();
//  if (m_expanding)
//  {
//    if (currZ <= maxHeight)
//    {
//      Point newP(geom.x(), geom.y(), currZ + 1);
//      m_graphic->setGeometry(newP);
//    }
//    else
//    {
//      m_expanding = false;
//      Point newP(geom.x(), geom.y(), currZ - 1);
//      m_graphic->setGeometry(newP);
//    }
//  }
//  else
//  {
//    if (currZ > 10)
//    {
//      Point newP(geom.x(), geom.y(), currZ - 1);
//      m_graphic->setGeometry(newP);
//    }
//    else
//    {
//      m_expanding = true;
//      Point newP(geom.x(), geom.y(), currZ + 1);
//      m_graphic->setGeometry(newP);
//    }
//  }

  // option 4 - changing selection color
  // for this approach it would be better to not pass the overlay to the alert
  // instead we should have a timer in the tool which created the overlay and have it
  // done once there
  // then alerts just opt into or out of selection

    QColor currColor = m_overlay->selectionColor();
    int currAlpha = currColor.alpha();
    constexpr int maxHeight = 245;
    if (m_expanding)
    {
      if (currAlpha < maxHeight)
      {
        currColor.setAlpha(currAlpha + 10);
        m_overlay->setSelectionColor(currColor);
      }
      else
      {
        m_expanding = false;
        currColor.setAlpha(currAlpha - 10);
        m_overlay->setSelectionColor(currColor);
      }
    }
    else
    {
      if (currAlpha > 10)
      {
        currColor.setAlpha(currAlpha - 10);
        m_overlay->setSelectionColor(currColor);
      }
      else
      {
        m_expanding = true;
        currColor.setAlpha(currAlpha + 10);
        m_overlay->setSelectionColor(currColor);
      }
    }

}
