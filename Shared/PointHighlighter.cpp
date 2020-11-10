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
#include "pch.hpp"

#include "PointHighlighter.h"

// toolkit headers
#include "ToolResourceProvider.h"

// C++ API headers
#include "GeoView.h"
#include "Graphic.h"
#include "GraphicListModel.h"
#include "GraphicsOverlay.h"
#include "Point.h"
#include "SimpleMarkerSceneSymbol.h"

// Qt headers
#include <QTimer>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::PointHighlighter
  \inmodule Dsa
  \inherits QObject
  \brief Manager for an animated highlight graphic centered on a point.
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
PointHighlighter::PointHighlighter(QObject* parent):
  QObject(parent)
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged,
          this, &PointHighlighter::onGeoViewChanged);

  onGeoViewChanged();
}

/*!
  \brief Destructor.
 */
PointHighlighter::~PointHighlighter()
{

}

/*!
  \brief Handle changes in the \a point to be highlighted.
 */
void PointHighlighter::onPointChanged(const Point& point)
{
  m_point = point;
}

/*!
  \brief Start the highlight.
 */
void PointHighlighter::startHighlight()
{
  if (m_point.isEmpty())
    return;

  if (!m_highlightOverlay || !m_highlightSymbol)
    return;

  Graphic* highlightGraphic = new Graphic(m_point, m_highlightSymbol, this);
  m_highlightOverlay->graphics()->append(highlightGraphic);

  if (m_highlightTimer)
  {
    disconnect(m_timerConnection);
    m_highlightTimer->stop();
    delete m_highlightTimer;
    m_highlightTimer = nullptr;
  }

  m_highlightTimer = new QTimer(this);
  m_timerConnection = connect(m_highlightTimer, &QTimer::timeout, this, [this]()
  {
    if (!m_highlightSymbol)
    {
      m_highlightTimer->stop();
      return;
    }

    if (!m_highlightOverlay || !m_highlightOverlay->graphics() || m_highlightOverlay->graphics()->isEmpty())
    {
      m_highlightTimer->stop();
      return;
    }

    Graphic* graphic = m_highlightOverlay->graphics()->first();
    if (!graphic)
    {
      m_highlightTimer->stop();
      return;
    }

    graphic->setGeometry(m_point);

    const int currDim = m_highlightSymbol->width();
    constexpr int maxDimension = 1000;
    int newDimension = currDim;
    float newOpacity = m_highlightOverlay->opacity();
    if (currDim < maxDimension)
    {
      newDimension += 10;
      newOpacity -= 0.01f;
    }
    else
    {
      newDimension = 1;
    }

    if (currDim > 10 && currDim < 30)
      newOpacity = 1.0f;

    m_highlightSymbol->setWidth(newDimension);
    m_highlightSymbol->setHeight(newDimension);
    m_highlightSymbol->setDepth(newDimension);
    m_highlightOverlay->setOpacity(newOpacity);
  });

  m_highlightTimer->start(10);
}

/*!
  \brief Stop the highlight.
 */
void PointHighlighter::stopHighlight()
{
  if (!m_highlightOverlay || !m_highlightOverlay->graphics() || m_highlightOverlay->graphics()->isEmpty())
    return;

  Graphic* graphic = m_highlightOverlay->graphics()->first();
  if (graphic)
  {
    delete graphic;
    m_highlightOverlay->graphics()->clear();
  }

  if (m_highlightTimer)
  {
    disconnect(m_timerConnection);
    m_highlightTimer->stop();
    delete m_highlightTimer;
    m_highlightTimer = nullptr;
  }
}

/*!
  \brief Handle changes to the goeView for highlighting.
 */
void PointHighlighter::onGeoViewChanged()
{
  if (m_highlightOverlay)
  {
    delete m_highlightOverlay;
    m_highlightOverlay = nullptr;
  }

  if (m_highlightSymbol)
  {
    delete m_highlightSymbol;
    m_highlightSymbol = nullptr;
  }

  GeoView* geoview = ToolResourceProvider::instance()->geoView();
  if (!geoview)
    return;

  if (!geoview->graphicsOverlays())
    return;

  m_highlightOverlay = new GraphicsOverlay(this);
  geoview->graphicsOverlays()->append(m_highlightOverlay);

  m_highlightSymbol = new SimpleMarkerSceneSymbol(
        SimpleMarkerSceneSymbolStyle::Sphere, Qt::red, 1.0, 1.0, 1.0, SceneSymbolAnchorPosition::Center, this);
}

} // Dsa
