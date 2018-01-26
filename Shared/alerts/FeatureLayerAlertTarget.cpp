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

#include "FeatureLayerAlertTarget.h"

#include "FeatureLayer.h"

using namespace Esri::ArcGISRuntime;

struct FeatureQueryResultManager {

  FeatureQueryResult* m_results;

  FeatureQueryResultManager(FeatureQueryResult* results):
    m_results(results)
  {
  }

  ~FeatureQueryResultManager()
  {
    delete m_results;
    m_results = nullptr;
  }
};

FeatureLayerAlertTarget::FeatureLayerAlertTarget(FeatureLayer* featureLayer):
  AlertTarget(featureLayer),
  m_FeatureLayer(featureLayer)
{
  // assume no editing of feature table

  FeatureTable* table = m_FeatureLayer->featureTable();
  QueryParameters allFeaturesQuery;
  allFeaturesQuery.setWhereClause("1=1");
  allFeaturesQuery.setReturnGeometry(true);

  connect(table, &FeatureTable::queryFeaturesCompleted, this, &FeatureLayerAlertTarget::handleQueryFeaturesCompleted);
  table->queryFeatures(allFeaturesQuery);
}

FeatureLayerAlertTarget::~FeatureLayerAlertTarget()
{

}

QList<Esri::ArcGISRuntime::Geometry> FeatureLayerAlertTarget::location() const
{
  if (!m_geomCache.isEmpty())
    return m_geomCache;

  auto it = m_features.begin();
  auto itEnd = m_features.end();
  for (; it != itEnd; ++it)
  {
    Feature* feat = *it;
    if (!feat)
      continue;

    m_geomCache.append(feat->geometry());
  }

  return m_geomCache;
}

void FeatureLayerAlertTarget::handleQueryFeaturesCompleted(QUuid, FeatureQueryResult* queryResults)
{
  FeatureQueryResultManager results(queryResults);
  if (!results.m_results)
  {
    emit locationChanged();
    return;
  }

  m_geomCache.clear();
  m_features = results.m_results->iterator().features(this);
  auto it = m_features.begin();
  auto itEnd = m_features.end();
  for (; it != itEnd; ++it)
  {
    Feature* feature = *it;
    if (feature)
      continue;

    connect(feature, &Feature::geometryChanged, this, [this]()
    {
      m_geomCache.clear();
      emit locationChanged();
    });
  }

  emit locationChanged();
}
