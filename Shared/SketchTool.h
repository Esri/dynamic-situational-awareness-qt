#ifndef SKETCHTOOL_H
#define SKETCHTOOL_H

#include <QColor>
#include <QList>

#include "AbstractTool.h"
#include "Geometry.h"
#include "GeometryTypes.h"
#include "Point.h"

namespace Esri {
  namespace ArcGISRuntime {
    class GeoView;
    class GraphicsOverlay;
    class GeometryBuilder;
    class Graphic;
    class Symbol;
  }
}

class SketchTool : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:
  explicit SketchTool(QObject* parent = nullptr);
  ~SketchTool();

  void initGeometryBuilder();
  void replaceGeometry(Esri::ArcGISRuntime::Geometry geometry);
  Esri::ArcGISRuntime::Geometry builderGeometry() const;

  bool isMultiPart() const;
  virtual Esri::ArcGISRuntime::GeometryType geometryType() const = 0;

  void clear();
  int addPart();
  void selectPartByIndex(int partIndex);
  void insertPointInPart(int partIndex, int pointIndex, Esri::ArcGISRuntime::Point drawPoint);
  Esri::ArcGISRuntime::Point normalizedPoint(double x, double y);
  virtual void updateSketch() = 0;

  // Functions that should be from the SketchEditor
  void setSketchSymbol(Esri::ArcGISRuntime::Symbol* symbol);
  Esri::ArcGISRuntime::Symbol* sketchSymbol();

protected:
  QList<Esri::ArcGISRuntime::Graphic*> m_partOutlineGraphics;
  Esri::ArcGISRuntime::GraphicsOverlay* m_sketchOverlay = nullptr;
  Esri::ArcGISRuntime::GeometryBuilder* m_geometryBuilder = nullptr;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  int m_selectedPartIndex = 0;

  // members that should be from the SketchEditor
  Esri::ArcGISRuntime::Symbol* m_sketchSymbol = nullptr;
};

#endif // SketchTool_H
