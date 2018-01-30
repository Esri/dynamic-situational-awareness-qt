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

/*!
  \internal
 */
struct GeometryQuadtree::QuadTree
{
  explicit  QuadTree(int level, double xMin, double xMax, double yMin, double yMax, int maxLevel);
  ~QuadTree();

  bool assign(const Geometry& geoElement, int geomId);
  void prune();

  QSet<int> intersectingIndices(const Envelope& extent) const;
  QSet<int> intersectingIndices(const Point& location) const;

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
  QSet<int> m_geometryIndices;
};

/*!
  \brief Constructor taking the \a extent of the Quadtree, the list of \a geoElements
  which the tree should include, the \a maxLevels for the tree and an optional \a parent.
 */
GeometryQuadtree::GeometryQuadtree(const Esri::ArcGISRuntime::Envelope& extent,
                                   const QList<Esri::ArcGISRuntime::GeoElement*>& geoElements,
                                   int maxLevels,
                                   QObject* parent):
  QObject(parent),
  m_maxLevels(maxLevels),
  m_elements(geoElements)
{
  // connect to the geometryChanged signal of individual GeoElements
  for (const auto& element : geoElements)
  {
    connect(element, &GeoElement::geometryChanged, this, [this]()
    {
      cacheGeometry();
      const Envelope newExtent = GeometryEngine::combineExtents(m_geometry);
      buildTree(newExtent);
    });
  }

  cacheGeometry();
  buildTree(extent);
}

/*!
  \brief Destructor.
 */
GeometryQuadtree::~GeometryQuadtree()
{

}

/*!
  \brief Returns the list of \l Geometry objects which are in quadtree cells which intersect \a extent

  \note No intersection test is carried out between the supplied Envelope and the results. For exact results,
  you should perform the desired geometry tests on the list of \l Geometry objects returned.
 */
QList<Geometry> GeometryQuadtree::candidateIntersections(const Envelope& extent) const
{
  // ensure the extent is in WGS84
  const Envelope wgs84 = GeometryEngine::project(extent, SpatialReference::wgs84());

  // obtain the indices of Geometry objects from quadtree nodes which intersect the extent
  QSet<int> geomIndexes = m_tree->intersectingIndices(wgs84);

  // collect the Geometry objects with an intersecting index
  QList<Geometry> results;
  for(const int idx: geomIndexes)
  {
    if (idx < m_geometry.size())
      results.push_back(m_geometry.at(idx));
  }

  return results;
}

/*!
  \brief Returns the list of \l Geometry objects which are in quadtree cells which intersect \a location

  \note No intersection test is carried out between the supplied point and the results. For exact results,
  you should perform the desired geometry tests on the list of \l Geometry objects returned.
 */
QList<Geometry> GeometryQuadtree::candidateIntersections(const Point& location) const
{
  // ensure the extent is in WGS84
  const Point wgs84 = GeometryEngine::project(location, SpatialReference::wgs84());

  // obtain the indices of Geometry objects from quadtree nodes which contain the location
  QSet<int> geomIndexes = m_tree->intersectingIndices(wgs84);

  // collect the Geometry objects with an intersecting index
  QList<Geometry> results;
  for(const int idx: geomIndexes)
  {
    if (idx < m_geometry.size())
      results.push_back(m_geometry.at(idx));
  }

  return results;
}

/*!
  \internal
 */
void GeometryQuadtree::buildTree(const Esri::ArcGISRuntime::Envelope& extent)
{
  // ensure the tree's extent is in WGS84
  const Envelope extentWgs84 = GeometryEngine::project(extent, SpatialReference::wgs84());

  // build the (currently empty) tree to the desired depth
  m_tree.reset(new QuadTree(0, extentWgs84.xMin(), extentWgs84.xMax(), extentWgs84.yMin(), extentWgs84.yMax(), m_maxLevels));

  // assign each geometry to the tree, alongwith its index in the m_geometry list
  int index = 0;
  for (const auto& geom : m_geometry)
  {
    const Geometry wgs84 = GeometryEngine::project(geom, SpatialReference::wgs84());
    m_tree->assign(wgs84, index);
    index++;
  }

  // remove any nodes from the tree which contain no geometry
  m_tree->prune();

  emit treeChanged();
}

/*!
  \internal
 */
void GeometryQuadtree::cacheGeometry()
{
  m_geometry.clear();

  // store a list of the geometry which the tree will use
  for (const auto& element : m_elements)
    m_geometry.append(element->geometry());
}

/*!
  \internal
 */
GeometryQuadtree::QuadTree::QuadTree(int level, double xMin, double xMax, double yMin, double yMax, int maxLevel):
  m_level(level),
  m_xMin(xMin),
  m_xMax(xMax),
  m_yMin(yMin),
  m_yMax(yMax)
{
  // if we have not reached the max depth of the tree, add the child nodes
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

/*!
  \internal
 */
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

/*!
  \internal
 */
bool GeometryQuadtree::QuadTree::assign(const Geometry& geometry, int geomIndex)
{
  // if the extent of the incoming geometry does not lie within this node, return
  const Envelope extent = geometry.extent();
  if (!intersects(extent))
    return false;

  // record this geometry index
  m_geometryIndices.insert(geomIndex);

  // (recursively) attempt to assign the geomeytry to each child node
  if (m_tl)
    m_tl->assign(geometry, geomIndex);
  if (m_tr)
    m_tr->assign(geometry, geomIndex);
  if (m_bl)
    m_bl->assign(geometry, geomIndex);
  if (m_br)
    m_br->assign(geometry, geomIndex);

  return true;
}

/*!
  \internal
 */
void GeometryQuadtree::QuadTree::prune()
{
  // for each existing child node,
  if (m_tl)
  {
    // remove the node (and alal of it's children) if it is empty
    if (m_tl->m_geometryIndices.empty())
    {
      delete m_tl;
      m_tl = nullptr;
    }
    // (recursively) call prune on the child
    else
    {
      m_tl->prune();
    }
  }

  if (m_tr)
  {
    if (m_tr->m_geometryIndices.empty())
    {
      delete m_tr;
      m_tr = nullptr;
    }
    else
    {
      m_tr->prune();
    }
  }

  if (m_bl)
  {
    if (m_bl->m_geometryIndices.empty())
    {
      delete m_bl;
      m_bl = nullptr;
    }
    else
    {
      m_bl->prune();
    }
  }

  if (m_br)
  {
    if (m_br->m_geometryIndices.empty())
    {
      delete m_br;
      m_br = nullptr;
    }
    else
    {
      m_br->prune();
    }
  }
}

/*!
  \internal
 */
QSet<int> GeometryQuadtree::QuadTree::intersectingIndices(const Envelope& extent) const
{
  // if this node contains no geometry indices there is no intersection
  if (m_geometryIndices.empty())
    return QSet<int>();

  // if this node does not intersect with the supplied extent, there is no intersection
  if (!intersects(extent))
    return QSet<int>();

  // if this node intesects but has no children, it must be a leaf node: return alal geometry indices
  if (!m_tl && !m_tr && !m_bl && !m_br)
    return m_geometryIndices;

  // for each existing child node, (recursively) build up the set of intersecting indices
  QSet<int> result;
  if (m_tl)
    result += m_tl->intersectingIndices(extent);

  if (m_tr)
    result += m_tr->intersectingIndices(extent);

  if (m_bl)
    result += m_bl->intersectingIndices(extent);

  if (m_br)
    result += m_br->intersectingIndices(extent);

  return result;
}

/*!
  \internal
 */
QSet<int> GeometryQuadtree::QuadTree::intersectingIndices(const Point& location) const
{
  // if this node contains no geometry indices there is no intersection
  if (m_geometryIndices.empty())
    return QSet<int>();

  // if this node does not intersect with the supplied extent, there is no intersection
  if (!intersects(location))
    return QSet<int>();

  // if this node intesects but has no children, it must be a leaf node: return alal geometry indices
  if (!m_tl && !m_tr && !m_bl && !m_br)
    return m_geometryIndices;

  // for each existing child node, (recursively) build up the set of intersecting indices
  QSet<int> result;
  if (m_tl)
    result += m_tl->intersectingIndices(location);

  if (m_tr)
    result += m_tr->intersectingIndices(location);

  if (m_bl)
    result += m_bl->intersectingIndices(location);

  if (m_br)
    result += m_br->intersectingIndices(location);

  return result;
}

/*!
  \internal
 */
bool GeometryQuadtree::QuadTree::intersects(const Envelope& extent) const
{
  // return whether the supplied extent overlaps this cell
  return (extent.xMin() < m_xMax &&
          extent.xMax() > m_xMin &&
          extent.yMin() < m_yMax &&
          extent.yMax() > m_yMin);
}

/*!
  \internal
 */
bool GeometryQuadtree::QuadTree::intersects(const Point& location) const
{
  // return whether the supplied location lies within this cell
  return (location.x() <= m_xMax &&
          location.x() >= m_xMin &&
          location.y() <= m_yMax &&
          location.y() >= m_yMin);
}
