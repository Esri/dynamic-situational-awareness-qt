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

#ifndef FEATURELAYERALERTTARGET_H
#define FEATURELAYERALERTTARGET_H

// dsa app headers
#include "AlertTarget.h"

// Qt headers
#include <QUuid>

namespace Esri {
namespace ArcGISRuntime {
class Feature;
class FeatureLayer;
class FeatureQueryResult;
}
}

namespace Dsa {

class GeometryQuadtree;

class FeatureLayerAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit FeatureLayerAlertTarget(Esri::ArcGISRuntime::FeatureLayer* FeatureLayer);
  ~FeatureLayerAlertTarget();

  QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;
  QVariant targetValue() const override;

private slots:
  void handleQueryFeaturesCompleted(QUuid taskId, Esri::ArcGISRuntime::FeatureQueryResult* featureQueryResult);

private:
  void rebuildQuadtree();

  Esri::ArcGISRuntime::FeatureLayer* m_FeatureLayer = nullptr;
  GeometryQuadtree* m_quadtree = nullptr;
  QList<Esri::ArcGISRuntime::Feature*> m_features;
  mutable QList<Esri::ArcGISRuntime::Geometry> m_geomCache;
};

} // Dsa

#endif // FEATURELAYERALERTTARGET_H
