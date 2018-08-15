
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

#include "AbstractSketchTool.h"

// C++ API headers
#include "GeoView.h"
#include "GeometryBuilder.h"
#include "GeometryTypes.h"
#include "Graphic.h"
#include "MapView.h"
#include "MultipartBuilder.h"
#include "MultipointBuilder.h"
#include "Point.h"
#include "PointBuilder.h"
#include "PolygonBuilder.h"
#include "PolylineBuilder.h"
#include "SceneView.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::AbstractSketchTool
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Abstract tool controller for working with sketches/markups.
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
AbstractSketchTool::AbstractSketchTool(QObject* parent) :
  AbstractTool(parent),
  m_sketchOverlay(new GraphicsOverlay(this))
{
  m_sketchOverlay->setOverlayId("Sketch overlay");
}

/*!
  \brief Destructor.
 */
AbstractSketchTool::~AbstractSketchTool()
{
}

/*!
  \brief Start a new \l Esri::ArcGISRuntime::PolylineBuilder for sketch geometry.
 */
void AbstractSketchTool::initGeometryBuilder()
{
  if (!m_geoView)
    return;

  if (geometryType() == GeometryType::Polyline)
    m_geometryBuilder = new PolylineBuilder(m_geoView->spatialReference(), this);
}

/*!
  \brief Return the \l Esri::ArcGISRuntime::Geometry for the current sketch.
 */
Geometry AbstractSketchTool::builderGeometry() const
{
  if (m_geometryBuilder)
    return m_geometryBuilder->toGeometry();

  return Geometry();
}

/*!
  \brief Sets the \l Esri::ArcGISRuntime::Symbol for the current sketch to \a symbol.
 */
void AbstractSketchTool::setSketchSymbol(Symbol* symbol)
{
  m_sketchSymbol = symbol;
}

/*!
  \brief Returns the \l Esri::ArcGISRuntime::Symbol for the current sketch.
 */
Symbol* AbstractSketchTool::sketchSymbol()
{
  return m_sketchSymbol;
}

/*!
  \brief Clears the current sketch.
 */
void AbstractSketchTool::clear()
{
  if (!m_geometryBuilder)
    return;

  if (isMultiPartBuilder())
  {
    MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
    multipartBuilder->parts()->removeAll();
  }
}

/*!
  \brief Converts the  screen coordinate (\a x, \a y) to an \l Esri::ArcGISRuntime::Point
  in coordinates of the map (2D) or base surface (3D).
 */
Point AbstractSketchTool::normalizedPoint(double x, double y)
{
  if (!m_geoView)
    return Point(x, y);

  if (m_geoView->geoViewType() == GeoViewType::MapView)
    return static_cast<MapView*>(m_geoView)->screenToLocation(x, y);
  else if (m_geoView->geoViewType() == GeoViewType::SceneView)
    return static_cast<SceneView*>(m_geoView)->screenToBaseSurface(x, y);

  return Point(x, y);
}

/*!
  \brief Selects the part in the current sketch geometry at \a partIndex.
 */
void AbstractSketchTool::selectPartByIndex(int partIndex)
{
  // clear selection
  m_sketchOverlay->clearSelection();
  m_selectedPartIndex = -1;

  if (!isMultiPartBuilder())
    return;

  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  if (partIndex < 0 || partIndex >= multipartBuilder->parts()->size())
    return;

  m_selectedPartIndex = partIndex;

  if (partIndex < m_partOutlineGraphics.size())
    m_partOutlineGraphics.at(partIndex)->setSelected(true);
}

/*!
  \brief Replaces the current sketch geometry with \a geometry.
 */
void AbstractSketchTool::replaceGeometry(Geometry geometry)
{
  m_geometryBuilder->replaceGeometry(geometry);
  updateSketch();
}

/*!
  \brief Returns whether the builder inherits from \l Esri::ArcGISRuntime::MultipartBuilder.
 */
bool AbstractSketchTool::isMultiPartBuilder() const
{
  if (!m_geometryBuilder)
    return false;

  const GeometryBuilderType type = m_geometryBuilder->geometryBuilderType();
  return (type == GeometryBuilderType::PolygonBuilder || type == GeometryBuilderType::PolylineBuilder);
}

/*!
  \brief Adds a new Part to the Builder. Returns the index of the added Part or \c -1 if invalid operation
 */
int AbstractSketchTool::addPart()
{
  if (!m_geometryBuilder || !isMultiPartBuilder())
    return -1;

  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  multipartBuilder->parts()->addPart(new Part(m_geometryBuilder->spatialReference(), this));

  return multipartBuilder->parts()->size() - 1;
}

/*!
  \brief Inserts a new \l Esri::ArcGISRuntime::Point \a drawPoint in the sketch at \a partIndex, \a pointIndex.
  Returns the index of the added Part or \c -1 if this is an invalid operation.
 */
void AbstractSketchTool::insertPointInPart(int partIndex, int pointIndex, const Point& drawPoint)
{
  if (!m_geometryBuilder)
    return;

  if (isMultiPartBuilder())
  {
    MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
    if (partIndex >= 0 && partIndex < multipartBuilder->parts()->size())
    {
      Part* part = multipartBuilder->parts()->part(partIndex);

      // for purposes of the freehand sketch, points will always be added to the end of the Part
      if (pointIndex >= 0 && pointIndex < part->pointCount())
        part->insertPoint(pointIndex, drawPoint);
      else
        part->addPoint(drawPoint);
    }
  }

  updateSketch();
}

/*!
  \brief Returns \l Esri::ArcGISRuntime::GraphicsOverlay for the sketch.
 */
GraphicsOverlay* AbstractSketchTool::sketchOverlay() const
{
  return m_sketchOverlay;
}

} // Dsa
