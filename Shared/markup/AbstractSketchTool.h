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

#ifndef ABSTRACTSKETCHTOOL_H
#define ABSTRACTSKETCHTOOL_H

// Qt headers
#include <QList>

// dsa headers
#include "AbstractTool.h"

namespace Esri::ArcGISRuntime {
  class GeoView;
  class GraphicsOverlay;
  class GeometryBuilder;
  class Graphic;
  class Symbol;
  class Geometry;
  class Point;
  enum class GeometryType;
}

namespace Dsa {

class AbstractSketchTool : public AbstractTool
{
  Q_OBJECT

public:
  explicit AbstractSketchTool(QObject* parent = nullptr);
  ~AbstractSketchTool();

  void initGeometryBuilder();
  void replaceGeometry(Esri::ArcGISRuntime::Geometry geometry);
  Esri::ArcGISRuntime::Geometry builderGeometry() const;

  bool isMultiPartBuilder() const;
  virtual Esri::ArcGISRuntime::GeometryType geometryType() const = 0;

  void clear();
  int addPart();
  void selectPartByIndex(int partIndex);
  void insertPointInPart(int partIndex, int pointIndex, const Esri::ArcGISRuntime::Point& drawPoint);
  Esri::ArcGISRuntime::Point normalizedPoint(double x, double y);

  Esri::ArcGISRuntime::GraphicsOverlay* sketchOverlay() const;

  // Functions that should be from the SketchEditor
  void setSketchSymbol(Esri::ArcGISRuntime::Symbol* symbol);
  Esri::ArcGISRuntime::Symbol* sketchSymbol();

protected:
  virtual void updateSketch() = 0;

  QList<Esri::ArcGISRuntime::Graphic*> m_partOutlineGraphics;
  Esri::ArcGISRuntime::GraphicsOverlay* m_sketchOverlay = nullptr;
  Esri::ArcGISRuntime::GeometryBuilder* m_geometryBuilder = nullptr;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  int m_selectedPartIndex = 0;

  // members that should be from the SketchEditor
  Esri::ArcGISRuntime::Symbol* m_sketchSymbol = nullptr;
};

} // Dsa

#endif // ABSTRACTSKETCHTOOL_H
