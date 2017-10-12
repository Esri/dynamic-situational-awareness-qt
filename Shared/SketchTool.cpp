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

#include "SketchTool.h"

#include "GeoView.h"
#include "SceneQuickView.h"
#include "GeometryBuilder.h"
#include "GeometryTypes.h"
#include "MultipartBuilder.h"
#include "MapQuickView.h"
#include "PolylineBuilder.h"
#include "Graphic.h"
#include "PointBuilder.h"
#include "PolygonBuilder.h"
#include "MultipointBuilder.h"

using namespace Esri::ArcGISRuntime;

SketchTool::SketchTool(QObject* parent) :
  AbstractTool(parent),
  m_sketchOverlay(new GraphicsOverlay(this))
{
}

SketchTool::~SketchTool()
{
}

void SketchTool::initGeometryBuilder()
{
  if (!m_geoView)
    return;

  // for now, only a PolylineBuilder is necessary
  switch (geometryType())
  {
    case GeometryType::Polyline:
      m_geometryBuilder = new PolylineBuilder(m_geoView->spatialReference(), this);
      break;
  }
}

Geometry SketchTool::builderGeometry() const
{
  if (m_geometryBuilder)
    return m_geometryBuilder->toGeometry();

  return Geometry();
}

void SketchTool::setSketchSymbol(Symbol* symbol)
{
  m_sketchSymbol = symbol;
}

Symbol *SketchTool::sketchSymbol()
{
  return m_sketchSymbol;
}

void SketchTool::clear()
{
  if (!m_geometryBuilder)
    return;

  // will add other cases when need arises
  switch (geometryType())
  {
    case GeometryType::Polyline:
    case GeometryType::Polygon:
      MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
      multipartBuilder->parts()->removeAll();
      break;
  }
}

// converts screen coordinates from screen to a Point in coordinates of the map (2D) or base surface (3D)
Point SketchTool::normalizedPoint(double x, double y)
{
  if (!m_geoView)
    return Point(x, y);

  if (m_geoView->geoViewType() == GeoViewType::MapView)
    return static_cast<MapQuickView*>(m_geoView)->screenToLocation(x, y);
  else if (m_geoView->geoViewType() == GeoViewType::SceneView)
    return static_cast<SceneQuickView*>(m_geoView)->screenToBaseSurface(x, y);

  return Point(x, y);
}

void SketchTool::selectPartByIndex(int partIndex)
{
  // clear selection
  m_sketchOverlay->clearSelection();
  m_selectedPartIndex = -1;

  if (!isMultiPart())
    return;

  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  if (partIndex < 0 || partIndex >= multipartBuilder->parts()->size())
    return;

  m_selectedPartIndex = partIndex;

  if (partIndex < m_partOutlineGraphics.size())
    m_partOutlineGraphics.at(partIndex)->setSelected(true);
}

void SketchTool::replaceGeometry(Geometry geometry)
{
  m_geometryBuilder->replaceGeometry(geometry);
  updateSketch();
}

// checks whether the builder inherits from MultipartBuilder
bool SketchTool::isMultiPart() const
{
  if (!m_geometryBuilder)
    return false;

  GeometryBuilderType type = m_geometryBuilder->geometryBuilderType();
  return (type == GeometryBuilderType::PolygonBuilder || type == GeometryBuilderType::PolylineBuilder);
}

// Adds a new Part to the Builder. Returns the index of the added Part or -1 if invalid operation
int SketchTool::addPart()
{
  if (!m_geometryBuilder || !isMultiPart())
    return -1;

  MultipartBuilder* multipartBuilder = static_cast<MultipartBuilder*>(m_geometryBuilder);
  multipartBuilder->parts()->addPart(new Part(m_geometryBuilder->spatialReference(), this));

  return multipartBuilder->parts()->size() - 1;
}

void SketchTool::insertPointInPart(int partIndex, int pointIndex, Point drawPoint)
{
  if (!m_geometryBuilder)
    return;

  // Only MultipartBuilder types need to be handled for telestrating
  switch (geometryType())
  {
    case GeometryType::Polyline:
    case GeometryType::Polygon:
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

      break;
  }

  updateSketch();
}
