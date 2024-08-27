/*******************************************************************************
 *  Copyright 2012-2024 Esri
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

#ifndef MESSAGESOVERLAYALERTTARGET_H
#define MESSAGESOVERLAYALERTTARGET_H

// dsa app headers
#include "AlertTarget.h"

namespace Esri::ArcGISRuntime {

class Geometry;
class Envelope;

}

namespace Dsa {

class MessagesOverlay;
class GeometryQuadtree;

class MessagesOverlayAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit MessagesOverlayAlertTarget(Dsa::MessagesOverlay* messagesOverlay);
  ~MessagesOverlayAlertTarget();

  QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;
  QVariant targetValue() const override;

private:
  Dsa::MessagesOverlay* m_messagesOverlay = nullptr;
  GeometryQuadtree* m_quadtree = nullptr;
  void rebuildQuadtree();
};

} // Dsa

#endif // MESSAGESOVERLAYALERTTARGET_H
