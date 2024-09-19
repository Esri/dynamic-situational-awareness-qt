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

// PCH header
#include "pch.hpp"

#include "FeatureLayerAlertTarget.h"

// dsa app headers
#include "FeatureQueryResultManager.h"
#include "GeometryQuadtree.h"

// C++ API headers
#include "Envelope.h"
#include "Feature.h"
#include "FeatureIterator.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "FeatureTable.h"
#include "QueryParameters.h"

// Qt headers
#include <QFuture>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::FeatureLayerAlertTarget
  \inmodule Dsa
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
  table->queryFeaturesAsync(allFeaturesQuery).then(this, [this](FeatureQueryResult* featureQueryResult)
  {
    // Store the results in a RAII manager to ensure they are cleaned up
    FeatureQueryResultManager results(featureQueryResult);
    if (!results.m_results)
    {
      emit dataChanged();
      return;
    }

    // cache all of the features
    m_geomCache.clear();
    m_features = results.m_results->iterator().features(this);
    for (auto* feature : m_features)
    {
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
  });
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
  for (auto* feature : m_features)
  {
    if (!feature)
      continue;

    m_geomCache.append(feature->geometry());
  }

  return m_geomCache;
}

/*!
  \brief Returns an empty QVariant.
 */
QVariant FeatureLayerAlertTarget::targetValue() const
{
  return QVariant{};
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

} // Dsa
