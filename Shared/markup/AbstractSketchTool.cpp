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
namespace Markup {

AbstractSketchTool::AbstractSketchTool(QObject* parent) :
  AbstractTool(parent),
  m_sketchOverlay(new GraphicsOverlay(this))
{
  m_sketchOverlay->setOverlayId("Sketch overlay");
}

AbstractSketchTool::~AbstractSketchTool()
{
}

void AbstractSketchTool::initGeometryBuilder()
{
  if (!m_geoView)
    return;

  if (geometryType() == GeometryType::Polyline)
    m_geometryBuilder = new PolylineBuilder(m_geoView->spatialReference(), this);
}

Geometry AbstractSketchTool::builderGeometry() const
{
  if (m_geometryBuilder)
    return m_geometryBuilder->toGeometry();

  return Geometry();
}

void AbstractSketchTool::setSketchSymbol(Symbol* symbol)
{
  m_sketchSymbol = symbol;
}

Symbol* AbstractSketchTool::sketchSymbol()
{
  return m_sketchSymbol;
}

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

// converts screen coordinates from screen to a Point in coordinates of the map (2D) or base surface (3D)
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

void AbstractSketchTool::replaceGeometry(Geometry geometry)
{
  m_geometryBuilder->replaceGeometry(geometry);
  updateSketch();
}

// checks whether the builder inherits from MultipartBuilder
bool AbstractSketchTool::isMultiPartBuilder() const
{
  if (!m_geometryBuilder)
    return false;

  const GeometryBuilderType type = m_geometryBuilder->geometryBuilderType();
  return (type == GeometryBuilderType::PolygonBuilder || type == GeometryBuilderType::PolylineBuilder);
}

// Adds a new Part to the Builder. Returns the index of the added Part or -1 if invalid operation
int AbstractSketchTool::addPart()
{
  if (!m_geometryBuilder || !isMultiPartBuilder())
    return -1;

  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  multipartBuilder->parts()->addPart(new Part(m_geometryBuilder->spatialReference(), this));

  return multipartBuilder->parts()->size() - 1;
}

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

GraphicsOverlay* AbstractSketchTool::sketchOverlay() const
{
  return m_sketchOverlay;
}

} // Markup
} // Dsa
