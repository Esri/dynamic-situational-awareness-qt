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

#ifndef GEOMETRYQUADTREE_H
#define GEOMETRYQUADTREE_H

#include <memory>

#include <QList>
#include <QObject>

namespace Esri {
namespace ArcGISRuntime {
class Envelope;
class GeoElement;
class Geometry;
class Graphic;
class Point;
}
}

struct QuadTree;

class GeometryQuadtree : public QObject
{
  Q_OBJECT

public:
  explicit GeometryQuadtree(const Esri::ArcGISRuntime::Envelope& extent,
                            const QList<Esri::ArcGISRuntime::GeoElement*>& geoElements,
                            QObject* parent = nullptr);
  ~GeometryQuadtree();

  QList<Esri::ArcGISRuntime::Geometry> intersections(const Esri::ArcGISRuntime::Envelope& extent) const;
  QList<Esri::ArcGISRuntime::Geometry> intersections(const Esri::ArcGISRuntime::Point& location) const;

private:
  std::unique_ptr<QuadTree> m_tree;
  QList<Esri::ArcGISRuntime::Geometry> m_geometry;
};

#endif // GEOMETRYQUADTREE_H
