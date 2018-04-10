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

#include "GeometryQuadtree.h"

// C++ API headers
#include "Envelope.h"
#include "GeoElement.h"
#include "GeometryEngine.h"
#include "Point.h"

// Qt headers
#include <QSet>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \internal
 */
struct GeometryQuadtree::QuadTree
{
  explicit QuadTree(int level, double xMin, double xMax, double yMin, double yMax);
  ~QuadTree();

  QuadTree* createTopLeft(int maxLevels);
  QuadTree* createTopRight(int maxLevels);
  QuadTree* createBottomLeft(int maxLevels);
  QuadTree* createBottomRight(int maxLevels);

  bool assign(const Envelope& extent, int geomId, int maxLevels);
  void prune();

  QSet<int> intersectingIds(const Envelope& extent) const;
  QSet<int> intersectingIds(const Point& location) const;

  bool contains(const Envelope& extent) const;
  bool intersects(const Envelope& extent) const;
  bool intersects(const Point& location) const;

  void removeId(int geomId);

  int m_level = 0;
  double m_xMin = 0.0;
  double m_xMax = 0.0;
  double m_yMin = 0.0;
  double m_yMax = 0.0;
  QuadTree* m_tl = nullptr; // top left
  QuadTree* m_tr = nullptr; // top right
  QuadTree* m_bl = nullptr; // bottom left
  QuadTree* m_br = nullptr; // bottom right
  QSet<int> m_geometryIds;
};

/*!
  \class GeometryQuadtree
  \inherits QObject
  \brief This class builds a Quadtree spatial structure covering a set of
  \l Esri::ArcGISRuntime::GeoElement objects.

  The tree then allows geometric tests for candifate intersections against
  query geometries.
 */

/*!
  \brief Constructor taking the \a extent of the Quadtree, the list of \a geoElements
  which the tree should include, the \a maxLevels for the tree and an optional \a parent.
 */
GeometryQuadtree::GeometryQuadtree(const Envelope& extent,
                                   const QList<GeoElement*>& geoElements,
                                   int maxLevels,
                                   QObject* parent):
  QObject(parent),
  m_maxLevels(maxLevels)
{
  // connect to the geometryChanged signal of individual GeoElements
  for (const auto& element : geoElements)
    handleNewGeoElement(element);

  buildTree(extent);
}

/*!
  \brief Destructor.
 */
GeometryQuadtree::~GeometryQuadtree()
{

}

/*!
  \brief Adds the \a newGeoElement into the quadtree.

  \note The tree will be re-built.
 */
void GeometryQuadtree::appendGeoElment(GeoElement* newGeoElement)
{
  if (!newGeoElement)
    return;

  const int newKey = handleNewGeoElement(newGeoElement);
  handleGeometryChange(newKey);
}

/*!
  \brief Returns the list of \l Geometry objects which are in quadtree cells which intersect \a geometry

  \note No intersection test is carried out between the supplied Geometry and the results. For exact results,
  you should perform the desired geometry tests on the list of \l Geometry objects returned.
 */
QList<Geometry> GeometryQuadtree::candidateIntersections(const Geometry& geometry) const
{
  return candidateIntersections(geometry.extent());
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
  QSet<int> geomIds = m_tree->intersectingIds(wgs84);

  // collect the Geometry objects with an intersecting Id
  QList<Geometry> results;
  for(const int id: geomIds)
  {
    // attempt to find the element Id in the lookup. If the element has been removed it may not be found
    auto findIt = m_elementStorage.find(id);
    if (findIt != m_elementStorage.end())
    {
      GeoElement* element = findIt.value();
      if (element)
        results.push_back(element->geometry());
    }
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
  QSet<int> geomIds = m_tree->intersectingIds(wgs84);

  // collect the Geometry objects with an intersecting Id
  QList<Geometry> results;
  for(const int id: geomIds)
  {
    // attempt to find the element Id in the lookup. If the element has been removed it may not be found
    auto findIt = m_elementStorage.find(id);
    if (findIt != m_elementStorage.end())
    {
      GeoElement* element = findIt.value();
      if (element)
        results.push_back(element->geometry());
    }
  }

  return results;
}

/*!
  \internal
 */
void GeometryQuadtree::buildTree(const Envelope& extent)
{
  // ensure the tree's extent is in WGS84
  const Envelope extentWgs84 = GeometryEngine::project(extent, SpatialReference::wgs84());

  // build the (currently empty) tree to the desired depth
  m_tree.reset(new QuadTree(0, extentWgs84.xMin(), extentWgs84.xMax(), extentWgs84.yMin(), extentWgs84.yMax()));

  // assign the geometry of each element to the tree, along with its id in the lookup
  auto it = m_elementStorage.cbegin();
  auto itEnd = m_elementStorage.cend();
  for (; it != itEnd; ++it)
  {
    GeoElement* element = it.value();
    if (!element)
      continue;

    const Geometry wgs84 = GeometryEngine::project(element->geometry(), SpatialReference::wgs84());
    m_tree->assign(wgs84.extent(), it.key(), m_maxLevels);
  }

  // remove any nodes from the tree which contain no geometry
  m_tree->prune();

  emit treeChanged();
}

/*!
  \internal
 */
void GeometryQuadtree::handleGeometryChange(int changedId)
{
  const GeoElement* changedElement = m_elementStorage.value(changedId);
  if (!changedElement)
    return;

  const Geometry wgs84Geom = GeometryEngine::project(changedElement->geometry(), SpatialReference::wgs84());
  const Envelope wgs84Extent = wgs84Geom.extent();

  // if the extent of the changed geom is the same or smaller than the existing tree, it can still be used
  if (m_tree->m_xMin <= wgs84Extent.xMin() &&
      m_tree->m_xMax >= wgs84Extent.xMax() &&
      m_tree->m_yMin <= wgs84Extent.yMin() &&
      m_tree->m_yMax >= wgs84Extent.yMax())
  {
    m_tree->removeId(changedId);
    m_tree->assign(wgs84Geom.extent(), changedId, m_maxLevels);
    m_tree->prune();
    emit treeChanged();
  }
  // otherwise calculate the new extent and rebuild the tree
  else
  {
    QList<Geometry> allGeom;
    for(auto it = m_elementStorage.begin(); it != m_elementStorage.end(); ++it)
    {
      GeoElement* element = it.value();
      if (!element)
        continue;

      if (element->geometry().isEmpty())
        continue;

      allGeom.append(GeometryEngine::project(element->geometry(), SpatialReference::wgs84()));
    }

    const Geometry newExtent = GeometryEngine::combineExtents(allGeom);
    buildTree(newExtent);
  }
}

/*!
  \internal

  Attempts to add the new \a geoElement into the storage QHash and connects to changes.

  Returns the key in the storage or \c -1 if unsuccesful.
 */
int GeometryQuadtree::handleNewGeoElement(GeoElement* geoElement)
{
  if (!geoElement)
    return -1;

  m_elementStorage.insert(m_nextKey, geoElement);
  const int insertedKey = m_nextKey;
  m_nextKey++;

  connect(geoElement, &GeoElement::geometryChanged, this, [this, geoElement]()
  {
    auto it = m_elementStorage.cbegin();
    auto itEnd = m_elementStorage.cend();
    for (; it != itEnd; ++it)
    {
      GeoElement* testElement = it.value();
      if (!testElement)
        continue;

      if (testElement == geoElement)
      {
        handleGeometryChange(it.key());
        return;
      }
    }
  });

  connect(geoElement, &GeoElement::destroyed, this, [this, geoElement]()
  {
    auto it = m_elementStorage.cbegin();
    auto itEnd = m_elementStorage.cend();
    for (; it != itEnd; ++it)
    {
      GeoElement* testElement = it.value();
      if (!testElement)
        continue;

      if (testElement == geoElement)
      {
        m_tree->removeId(it.key());
        m_tree->prune();
        m_elementStorage.remove(it.key());
        emit treeChanged();
        return;
      }
    }
  });

  return insertedKey;
}

/*!
  \internal
 */
GeometryQuadtree::QuadTree::QuadTree(int level, double xMin, double xMax, double yMin, double yMax):
  m_level(level),
  m_xMin(xMin),
  m_xMax(xMax),
  m_yMin(yMin),
  m_yMax(yMax)
{

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

GeometryQuadtree::QuadTree* GeometryQuadtree::QuadTree::createTopLeft(int maxLevels)
{
  // if we have not reached the max depth of the tree, add the child nodes
  if (m_level > maxLevels)
    return nullptr;

  const double xMid = ((m_xMax - m_xMin) * 0.5) + m_xMin;
  const double yMid = ((m_yMax - m_yMin) * 0.5) + m_yMin;

  return new QuadTree(m_level +1, m_xMin, xMid, yMid, m_yMax);
}

GeometryQuadtree::QuadTree* GeometryQuadtree::QuadTree::createTopRight(int maxLevels)
{
  // if we have not reached the max depth of the tree, add the child nodes
  if (m_level > maxLevels)
    return nullptr;

  const double xMid = ((m_xMax - m_xMin) * 0.5) + m_xMin;
  const double yMid = ((m_yMax - m_yMin) * 0.5) + m_yMin;

  return new QuadTree(m_level + 1, xMid, m_xMax, yMid, m_yMax);
}

GeometryQuadtree::QuadTree* GeometryQuadtree::QuadTree::createBottomLeft(int maxLevels)
{
  // if we have not reached the max depth of the tree, add the child nodes
  if (m_level > maxLevels)
    return nullptr;

  const double xMid = ((m_xMax - m_xMin) * 0.5) + m_xMin;
  const double yMid = ((m_yMax - m_yMin) * 0.5) + m_yMin;

  return new QuadTree(m_level + 1, m_xMin, xMid, m_yMin, yMid);
}

GeometryQuadtree::QuadTree* GeometryQuadtree::QuadTree::createBottomRight(int maxLevels)
{
  // if we have not reached the max depth of the tree, add the child nodes
  if (m_level > maxLevels)
    return nullptr;

  const double xMid = ((m_xMax - m_xMin) * 0.5) + m_xMin;
  const double yMid = ((m_yMax - m_yMin) * 0.5) + m_yMin;

  return new QuadTree(m_level + 1, xMid, m_xMax, m_yMin, yMid);
}

/*!
  \internal
 */
bool GeometryQuadtree::QuadTree::assign(const Envelope& extent, int geomIndex, int maxLevels)
{
  // if the extent of the incoming geometry does not lie within this node, return
  if (!intersects(extent))
    return false;

  // record this geometry index
  m_geometryIds.insert(geomIndex);

  // (recursively) attempt to assign the geomeytry to each child node
  // if the node already exists, just assign
  if (m_tl)
  {
    m_tl->assign(extent, geomIndex, maxLevels);
  }
  // otherwise, create a temporary node and only keep it if it will contain this geometry
  else
  {
    QuadTree* temp = createTopLeft(maxLevels);
    if (temp && temp->assign(extent, geomIndex, maxLevels))
      m_tl = temp;
    else
      delete temp;
  }

  if (m_tr)
  {
    m_tr->assign(extent, geomIndex, maxLevels);
  }
  else
  {
    QuadTree* temp = createTopRight(maxLevels);
    if (temp && temp->assign(extent, geomIndex, maxLevels))
      m_tr = temp;
    else
      delete temp;
  }

  if (m_bl)
  {
    m_bl->assign(extent, geomIndex, maxLevels);
  }
  else
  {
    QuadTree* temp = createBottomLeft(maxLevels);
    if (temp && temp->assign(extent, geomIndex, maxLevels))
      m_bl = temp;
    else
      delete temp;
  }

  if (m_br)
  {
    m_br->assign(extent, geomIndex, maxLevels);
  }
  else
  {
    QuadTree* temp = createBottomRight(maxLevels);
    if (temp && temp->assign(extent, geomIndex, maxLevels))
      m_br = temp;
    else
      delete temp;
  }

  return true;
}

/*!
  \internal
 */
void GeometryQuadtree::QuadTree::prune()
{
  // for each existing child node remove the node (and any children if they are empty)
  if (m_tl)
  {
    // remove the node (and all of it's children) if it is empty
    if (m_tl->m_geometryIds.empty())
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
    if (m_tr->m_geometryIds.empty())
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
    if (m_bl->m_geometryIds.empty())
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
    if (m_br->m_geometryIds.empty())
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
QSet<int> GeometryQuadtree::QuadTree::intersectingIds(const Envelope& extent) const
{
  // if this node contains no geometry indices there is no intersection
  if (m_geometryIds.empty())
    return QSet<int>();

  // if this node does not intersect with the supplied extent, there is no intersection
  if (!intersects(extent))
    return QSet<int>();

  // if this node intersects but has no children, it must be a leaf node: return all geometry indices
  if (!m_tl && !m_tr && !m_bl && !m_br)
    return m_geometryIds;

  // for each existing child node, (recursively) build up the set of intersecting indices
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

/*!
  \internal
 */
QSet<int> GeometryQuadtree::QuadTree::intersectingIds(const Point& location) const
{
  // if this node contains no geometry indices, there is no intersection
  if (m_geometryIds.empty())
    return QSet<int>();

  // if this node does not intersect with the supplied extent, there is no intersection
  if (!intersects(location))
    return QSet<int>();

  // if this node intesects but has no children, it must be a leaf node: return all geometry indices
  if (!m_tl && !m_tr && !m_bl && !m_br)
    return m_geometryIds;

  // for each existing child node, (recursively) build up the set of intersecting indices
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

/*!
  \internal
 */
bool GeometryQuadtree::QuadTree::contains(const Envelope& extent) const
{
  return (extent.xMin() >= m_xMin &&
          extent.xMax() <= m_xMax &&
          extent.yMin() >= m_yMin &&
          extent.yMax() <= m_yMax);
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

void GeometryQuadtree::QuadTree::removeId(int index)
{
  if (m_geometryIds.remove(index))
  {
    if (m_tl)
      m_tl->removeId(index);

    if (m_tr)
      m_tr->removeId(index);

    if (m_bl)
      m_bl->removeId(index);

    if (m_br)
      m_br->removeId(index);
  }
}

} // Dsa
