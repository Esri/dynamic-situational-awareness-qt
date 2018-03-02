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

#ifndef LOCATIONVIEWSHED360_H
#define LOCATIONVIEWSHED360_H

#include "Viewshed360.h"

namespace Esri {
  namespace ArcGISRuntime {
    class Point;
    class GraphicsOverlay;
    class Graphic;
  }
}

class LocationViewshed360 : public Viewshed360
{
  Q_OBJECT

public:
  LocationViewshed360(const Esri::ArcGISRuntime::Point& point,
                Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay,
                Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay, QObject* parent = nullptr);
  ~LocationViewshed360();

  Esri::ArcGISRuntime::Point point() const;
  void setPoint(const Esri::ArcGISRuntime::Point& point);

  void setVisible(bool visible) override;

  double heading() const override;
  void setHeading(double heading) override;

  double pitch() const override;
  void setPitch(double pitch) override;

protected:
  void update360Mode(bool is360Mode) override;

private:
  Q_DISABLE_COPY(LocationViewshed360)
  LocationViewshed360() = delete;

  QPointer<Esri::ArcGISRuntime::GraphicsOverlay> m_graphicsOverlay;
  Esri::ArcGISRuntime::Graphic* m_locationViewshedGraphic = nullptr;
};

#endif // LOCATIONVIEWSHED360_H
