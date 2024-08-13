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

#ifndef DYNAMICENTITYLAYERALERTTARGET_H
#define DYNAMICENTITYLAYERALERTTARGET_H

// dsa app headers
#include "AlertTarget.h"

namespace Esri::ArcGISRuntime {

class DynamicEntity;
class DynamicEntityInfo;
class DynamicEntityLayer;
class Graphic;

}

namespace Dsa {

class GeometryQuadtree;

class DynamicEntityLayerAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit DynamicEntityLayerAlertTarget(Esri::ArcGISRuntime::DynamicEntityLayer* dynamicEntityLayer);
  ~DynamicEntityLayerAlertTarget();

  QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;
  QVariant targetValue() const override;

private:
  Esri::ArcGISRuntime::DynamicEntityLayer* m_dynamicEntityLayer = nullptr;
  GeometryQuadtree* m_quadtree = nullptr;
  QHash<quint64, Esri::ArcGISRuntime::Graphic*> m_entityGraphics;
  void connectEntityGraphic(Esri::ArcGISRuntime::DynamicEntity* dynamicEntity);
  void rebuildQuadtree();
};

} // Dsa

#endif // DYNAMICENTITYLAYERALERTTARGET_H
