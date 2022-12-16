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

#ifndef GRAPHICSOVERLAYALERTTARGET_H
#define GRAPHICSOVERLAYALERTTARGET_H

// dsa app headers
#include "AlertTarget.h"

namespace Esri::ArcGISRuntime {
class Graphic;
class GraphicsOverlay;
}

namespace Dsa {

class GeometryQuadtree;

class GraphicsOverlayAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit GraphicsOverlayAlertTarget(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay);
  ~GraphicsOverlayAlertTarget();

  QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;
  QVariant targetValue() const override;

private:
  void setupGraphicConnections(Esri::ArcGISRuntime::Graphic* graphic);
  void rebuildQuadtree();

  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  GeometryQuadtree* m_quadtree = nullptr;
  QList<QMetaObject::Connection> m_graphicConnections;
};

} // Dsa

#endif // GRAPHICSOVERLAYALERTTARGET_H
