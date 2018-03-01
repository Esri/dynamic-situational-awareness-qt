// Copyright 2018 ESRI
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

#ifndef POINTVIEWSHED_H
#define POINTVIEWSHED_H

#include "AbstractViewshed.h"
#include "Point.h"

namespace Esri {
  namespace ArcGISRuntime {
    class Point;
    class GraphicsOverlay;
    class Graphic;
  }
}

class PointViewshed : public AbstractViewshed
{
  Q_OBJECT

public:
  PointViewshed(const Esri::ArcGISRuntime::Point& point,
                Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay,
                Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay, QObject* parent = nullptr);
  ~PointViewshed();

  Esri::ArcGISRuntime::Point point() const;
  void setPoint(const Esri::ArcGISRuntime::Point& point);

  void setVisible(bool visible) override;

  double heading() const override;
  void setHeading(double heading) override;

  double pitch() const override;
  void setPitch(double pitch) override;

  AnalysisType analysisType() const override;

protected:
  void update360Mode(bool is360Mode) override;

private:
  Q_DISABLE_COPY(PointViewshed)
  PointViewshed() = delete;

  QPointer<Esri::ArcGISRuntime::GraphicsOverlay> m_graphicsOverlay;
  Esri::ArcGISRuntime::Graphic* m_locationViewshedGraphic = nullptr;
};

#endif // POINTVIEWSHED_H
