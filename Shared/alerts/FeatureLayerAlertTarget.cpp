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

// PCH header
#include "pch.hpp"

#include "FeatureLayerAlertTarget.h"

// example app headers
#include "FeatureQueryResultManager.h"
#include "GeometryQuadtree.h"

// C++ API headers
#include "FeatureLayer.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa
{
namespace Alerts
{

/*!
  \class FeatureLayerAlertTarget
  \inherits AlertTarget
  \brief Represents a target based on an \l Esri::ArcGISRuntime::FeatureLayer
  for an \l AlertCondition.

  Changes to any of the features in the layer will cause the \l AlertTarget::locationChanged
  signal to be emitted.
  */

/*!
  \brief Constructor taking an \l Esri::ArcGISRuntime::FeatureLayer (\a featureLayer).

  All features will be retrieved from the underlying feature layer.
 */
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

/*!
  \brief Destructor.
 */
FeatureLayerAlertTarget::~FeatureLayerAlertTarget()
{

}

/*!
  \brief Returns the list of \l Esri::ArcGISRuntime::Geometry which are in the \a targetArea.

  \note No exact intersection tests are carried out to create this list.
 */
QList<Geometry> FeatureLayerAlertTarget::targetGeometries(const Envelope& targetArea) const
{
  // if the quad-tree has been built use it to determine the candidate geometry
  if (m_quadtree)
    return m_quadtree->candidateIntersections(targetArea);

  // if there is no quad-tree just return the cache of geoemtry
  if (!m_geomCache.isEmpty())
    return m_geomCache;

  // if there is no cached geometry, get the geometry for all features
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

/*!
  \brief Returns an empty QVariant.
 */
QVariant FeatureLayerAlertTarget::targetValue() const
{
  return QVariant();
}

/*!
  \brief internal.

  Handle the query to obtain all of the features in the layer.
 */
void FeatureLayerAlertTarget::handleQueryFeaturesCompleted(QUuid, FeatureQueryResult* queryResults)
{
  // Store the results in a RAII manager to ensure they are cleaned up
  Utilities::FeatureQueryResultManager results(queryResults);
  if (!results.m_results)
  {
    emit dataChanged();
    return;
  }

  // cache all of the features
  m_geomCache.clear();
  m_features = results.m_results->iterator().features(this);
  auto it = m_features.begin();
  auto itEnd = m_features.end();
  for (; it != itEnd; ++it)
  {
    Feature* feature = *it;
    if (!feature)
      continue;

    // for each feature, connect to the geometryChanged signal
    connect(feature, &Feature::geometryChanged, this, [this]()
    {
      m_geomCache.clear();
      rebuildQuadtree();
      emit dataChanged();
    });
  }

  rebuildQuadtree();
  emit dataChanged();
}

/*!
  \brief internal.

  Build the quadtree used to find intersections with feature geometry etc.
 */
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

} // Alerts
} // Dsa
