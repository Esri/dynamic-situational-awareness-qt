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

#ifndef FEATURELAYERALERTTARGET_H
#define FEATURELAYERALERTTARGET_H

namespace Esri
{
namespace ArcGISRuntime
{
class Feature;
class FeatureLayer;
class FeatureQueryResult;
}
}

#include "AlertTarget.h"

#include <QUuid>

class FeatureLayerAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit FeatureLayerAlertTarget(Esri::ArcGISRuntime::FeatureLayer* FeatureLayer);
  ~FeatureLayerAlertTarget();

  virtual QList<Esri::ArcGISRuntime::Geometry> targetGeometries() const override;

private slots:
  void handleQueryFeaturesCompleted(QUuid taskId, Esri::ArcGISRuntime::FeatureQueryResult* featureQueryResult);

private:
  Esri::ArcGISRuntime::FeatureLayer* m_FeatureLayer = nullptr;
  QList<Esri::ArcGISRuntime::Feature*> m_features;
  mutable QList<Esri::ArcGISRuntime::Geometry> m_geomCache;
};

#endif // FEATURELAYERALERTTARGET_H
