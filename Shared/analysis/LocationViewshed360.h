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

#ifndef LOCATIONVIEWSHED360_H
#define LOCATIONVIEWSHED360_H

// dsa app headers
#include "Viewshed360.h"

namespace Esri {
  namespace ArcGISRuntime {
    class Point;
    class GraphicsOverlay;
    class Graphic;
  }
}

namespace Dsa {

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

private:
  Q_DISABLE_COPY(LocationViewshed360)
  LocationViewshed360() = delete;

  QPointer<Esri::ArcGISRuntime::GraphicsOverlay> m_graphicsOverlay;
  Esri::ArcGISRuntime::Graphic* m_locationViewshedGraphic = nullptr;
};

} // Dsa

#endif // LOCATIONVIEWSHED360_H
