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
#include "GeometryQuadtree.h"

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

QList<Esri::ArcGISRuntime::Geometry> FeatureLayerAlertTarget::targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const
{
  if (m_quadtree)
    return m_quadtree->candidateIntersections(targetArea);

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
    if (!feature)
      continue;

    connect(feature, &Feature::geometryChanged, this, [this]()
    {
      m_geomCache.clear();
      rebuildQuadtree();
      emit locationChanged();
    });
  }

  rebuildQuadtree();
  emit locationChanged();
}

void FeatureLayerAlertTarget::rebuildQuadtree()
{
  if (m_quadtree)
  {
    delete m_quadtree;
    m_quadtree = nullptr;
  }

  QList<GeoElement*> elements;
  for (auto it = m_features.begin(); it != m_features.end(); ++it)
    elements.append(*it);

  if (elements.size() > 1)
    m_quadtree = new GeometryQuadtree(m_FeatureLayer->fullExtent(), elements, 8, this);
}
