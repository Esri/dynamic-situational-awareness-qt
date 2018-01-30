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

#include "Envelope.h"
#include "GeoElement.h"
#include "GeometryEngine.h"
#include "Point.h"

#include <QList>
#include <QSet>

using namespace Esri::ArcGISRuntime;

struct GeometryQuadtree::QuadTree
{
  explicit  QuadTree(int level, double xMin, double xMax, double yMin, double yMax, int maxLevel);
  ~QuadTree();

  bool assign(const Geometry& geoElement, int geomId);
  void prune();

  QSet<int> intersectingIds(const Envelope& extent) const;
  QSet<int> intersectingIds(const Point& location) const;

  bool intersects(const Envelope& extent) const;
  bool intersects(const Point& location) const;

  int m_level = 0;
  double m_xMin = 0.0;
  double m_xMax = 0.0;
  double m_yMin = 0.0;
  double m_yMax = 0.0;
  QuadTree* m_tl = nullptr; // top left
  QuadTree* m_tr = nullptr; // top right
  QuadTree* m_bl = nullptr; // bottom left
  QuadTree* m_br = nullptr; // bottom right
  QSet<int> m_geometries;
};

GeometryQuadtree::GeometryQuadtree(const Esri::ArcGISRuntime::Envelope& extent,
                                   const QList<Esri::ArcGISRuntime::GeoElement*>& geoElements,
                                   QObject* parent):
  QObject(parent)
{
  const Envelope extentWgs84 = GeometryEngine::project(extent, SpatialReference::wgs84());
  m_tree.reset(new QuadTree(0, extentWgs84.xMin(), extentWgs84.xMax(), extentWgs84.yMin(), extentWgs84.yMax(), 4));

  for (const auto& element : geoElements)
    m_geometry.append(element->geometry());

  int i = 0;
  for (const auto& geom : m_geometry)
  {
    const Geometry wgs84 = GeometryEngine::project(geom, SpatialReference::wgs84());
    m_tree->assign(wgs84, i);
    i++;
  }

  m_tree->prune();
}

GeometryQuadtree::~GeometryQuadtree()
{

}

QList<Geometry> GeometryQuadtree::intersections(const Envelope& extent) const
{
  const Envelope wgs84 = GeometryEngine::project(extent, SpatialReference::wgs84());
  QSet<int> geomIndexes = m_tree->intersectingIds(wgs84);
  QList<Geometry> results;
  for(const int idx: geomIndexes)
  {
    if (idx < m_geometry.size())
      results.push_back(m_geometry.at(idx));
  }

  return results;
}

QList<Geometry> GeometryQuadtree::intersections(const Point &location) const
{
  const Point wgs84 = GeometryEngine::project(location, SpatialReference::wgs84());
  QSet<int> geomIndexes = m_tree->intersectingIds(wgs84);
  QList<Geometry> results;
  for(const int idx: geomIndexes)
  {
    if (idx < m_geometry.size())
      results.push_back(m_geometry.at(idx));
  }

  return results;
}

GeometryQuadtree::QuadTree::QuadTree(int level, double xMin, double xMax, double yMin, double yMax, int maxLevel):
  m_level(level),
  m_xMin(xMin),
  m_xMax(xMax),
  m_yMin(yMin),
  m_yMax(yMax)
{
  if (m_level <= maxLevel)
  {
    const int newLevel = m_level +1;
    const double xMid = ((m_xMax - m_xMin) * 0.5) + m_xMin;
    const double yMid = ((m_yMax - m_yMin) * 0.5) + m_yMin;
    m_tl = new QuadTree(newLevel, m_xMin, xMid, yMid, m_yMax, maxLevel);
    m_tr = new QuadTree(newLevel, xMid, m_xMax, yMid, m_yMax, maxLevel);
    m_bl = new QuadTree(newLevel, m_xMin, xMid, m_yMin, yMid, maxLevel);
    m_br = new QuadTree(newLevel, xMid, m_xMax, m_yMin, yMid, maxLevel);
  }
}

GeometryQuadtree::QuadTree::~QuadTree()
{
  if (m_tl)
    delete m_tl;

  if (m_tr)
    delete m_tr;

  if (m_bl)
    delete m_bl;

  if (m_br)
    delete m_br;
}

bool GeometryQuadtree::QuadTree::assign(const Geometry& geometry, int geomId)
{
  const Envelope extent = geometry.extent();
  if (!intersects(extent))
    return false;

  m_geometries.insert(geomId);

  if (m_tl)
    m_tl->assign(geometry, geomId);
  if (m_tr)
    m_tr->assign(geometry, geomId);
  if (m_bl)
    m_bl->assign(geometry, geomId);
  if (m_br)
    m_br->assign(geometry, geomId);

  return true;
}

void GeometryQuadtree::QuadTree::prune()
{
  if (m_tl)
  {
    if (m_tl->m_geometries.empty())
    {
      delete m_tl;
      m_tl = nullptr;
    }
    else
      m_tl->prune();
  }

  if (m_tr)
  {
    if (m_tr->m_geometries.empty())
    {
      delete m_tr;
      m_tr = nullptr;
    }
    else
      m_tr->prune();
  }

  if (m_bl)
  {
    if (m_bl->m_geometries.empty())
    {
      delete m_bl;
      m_bl = nullptr;
    }
    else
      m_bl->prune();
  }

  if (m_br)
  {
    if (m_br->m_geometries.empty())
    {
      delete m_br;
      m_br = nullptr;
    }
    else
      m_br->prune();
  }
}

QSet<int> GeometryQuadtree::QuadTree::intersectingIds(const Envelope& extent) const
{
  if (m_geometries.empty())
    return QSet<int>();

  if (!intersects(extent))
    return QSet<int>();

  if (!m_tl && !m_tr && !m_bl && !m_br)
    return m_geometries;

 QSet<int> result;
 if (m_tl)
    result += m_tl->intersectingIds(extent);

 if (m_tr)
    result += m_tr->intersectingIds(extent);

 if (m_bl)
    result += m_bl->intersectingIds(extent);

 if (m_br)
    result += m_br->intersectingIds(extent);

 return result;
}

QSet<int> GeometryQuadtree::QuadTree::intersectingIds(const Point& location) const
{
  if (!intersects(location))
    return QSet<int>();

  if (!m_tl && !m_tr && !m_bl && !m_br)
    return m_geometries;

 QSet<int> result;
 if (m_tl)
    result += m_tl->intersectingIds(location);

 if (m_tr)
    result += m_tr->intersectingIds(location);

 if (m_bl)
    result += m_bl->intersectingIds(location);

 if (m_br)
    result += m_br->intersectingIds(location);

 return result;
}

bool GeometryQuadtree::QuadTree::intersects(const Envelope& extent) const
{
  return (extent.xMin() < m_xMax &&
          extent.xMax() > m_xMin &&
          extent.yMin() < m_yMax &&
          extent.yMax() > m_yMin);
}

bool GeometryQuadtree::QuadTree::intersects(const Point& location) const
{
  return (location.x() <= m_xMax &&
          location.x() >= m_xMin &&
          location.y() <= m_yMax &&
          location.y() >= m_yMin);
}
