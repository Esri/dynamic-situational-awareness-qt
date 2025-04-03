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

#ifndef GEOMETRYQUADTREE_H
#define GEOMETRYQUADTREE_H

// Qt headers
#include <QHash>
#include <QList>
#include <QObject>

// STL headers
#include <memory>

namespace Esri::ArcGISRuntime {
  class Envelope;
  class GeoElement;
  class Geometry;
  class Point;
}

namespace Dsa {

class GeoElementSignaler;

class GeometryQuadtree : public QObject
{
  Q_OBJECT

public:
  GeometryQuadtree(const Esri::ArcGISRuntime::Envelope& extent,
                   const QList<Esri::ArcGISRuntime::GeoElement*>& geoElements,
                   int maxLevels,
                   QObject* parent = nullptr);
  ~GeometryQuadtree();

  void appendGeoElment(Esri::ArcGISRuntime::GeoElement* newGeoElement);

  QList<Esri::ArcGISRuntime::Geometry> candidateIntersections(const Esri::ArcGISRuntime::Geometry& geometry) const;
  QList<Esri::ArcGISRuntime::Geometry> candidateIntersections(const Esri::ArcGISRuntime::Envelope& extent) const;
  QList<Esri::ArcGISRuntime::Geometry> candidateIntersections(const Esri::ArcGISRuntime::Point& location) const;

signals:
  void treeChanged();

private:
  void buildTree(const Esri::ArcGISRuntime::Envelope& extent);
  void handleGeometryChange(int changedIndex);
  int handleNewGeoElement(Esri::ArcGISRuntime::GeoElement* geoElement);

  struct QuadTree;

  int m_maxLevels;
  std::unique_ptr<QuadTree> m_tree;
  QHash<int, GeoElementSignaler*> m_elementStorage;
  int m_nextKey = 0;
};

} // Dsa

#endif // GEOMETRYQUADTREE_H
