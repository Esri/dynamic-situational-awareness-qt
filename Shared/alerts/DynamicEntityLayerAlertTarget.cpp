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

#include <QMap>

#include "DynamicEntityLayerAlertTarget.h"

// dsa app headers
#include "GeometryQuadtree.h"
#include "MessageFeed.h"
#include "MessagesOverlay.h"

// C++ API headers
#include "DynamicEntity.h"
#include "DynamicEntityInfo.h"
#include "DynamicEntityLayer.h"
#include "DynamicEntityObservation.h"
#include "DynamicEntityObservationInfo.h"
#include "Envelope.h"
#include "Graphic.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::DynamicEntityLayerAlertTarget
  \inmodule Dsa
  \inherits AlertTarget
  \brief Represents a target based on an \l Esri::ArcGISRuntime::DynamicEntityLayer
  for an \l AlertCondition.

  Changes to any of the dynamic entities in the layer will cause the \l AlertTarget::locationChanged
  signal to be emitted.
  */


/*!
  \brief Constructor taking an \l Esri::ArcGISRuntime::DynamicEntityLayer (\a dynamicEntityLayer).

  Entities are added/updated/removed by the DynamicEntityDataSource of the DynamicEntityLayer.
 */
DynamicEntityLayerAlertTarget::DynamicEntityLayerAlertTarget(DynamicEntityLayer *dynamicEntityLayer) :
  AlertTarget(dynamicEntityLayer),
  m_dynamicEntityLayer(dynamicEntityLayer),
  m_entityGraphics(QMap<quint64, Graphic*>{})
{
  // subscribe to the entity received signal from the source of the dynamic layer
  connect(m_dynamicEntityLayer->dataSource(), &DynamicEntityDataSource::dynamicEntityReceived, this, [this](DynamicEntityInfo *info)
  {
    // check the new entity for a connection and mark the info as delete later
    auto dynamicEntity = info->dynamicEntity();
    connectEntityGraphic(dynamicEntity);
    info->deleteLater();
  });

  // subscribe to the observation received signal (this essentially serves as the 'update' signal for the dynamic layer type)
  connect(m_dynamicEntityLayer->dataSource(), &DynamicEntityDataSource::dynamicEntityObservationReceived, this, [this](DynamicEntityObservationInfo *observationInfo)
  {
    // check the entity for a connection and mark the observation as delete later
    auto dynamicEntity = observationInfo->observation()->dynamicEntity();
    connectEntityGraphic(dynamicEntity);
    observationInfo->deleteLater();
  });

  // subscribe to the purged signal to remove any graphics from the lookup
  connect(m_dynamicEntityLayer->dataSource(), &DynamicEntityDataSource::dynamicEntityPurged, this, [this](DynamicEntityInfo *info)
  {
    // ensure the graphic is actually in the lookup
    auto dynamicEntity = info->dynamicEntity();
    if (m_entityGraphics.contains(dynamicEntity->entityId()))
    {
      // get a cleanup reference to the graphic that was stored and remove it from the lookup
      std::unique_ptr<Graphic> graphic(m_entityGraphics[dynamicEntity->entityId()]);
      m_entityGraphics.remove(dynamicEntity->entityId());

      // signal the data has changed and rebuild the quad tree
      rebuildQuadtree();
      emit dataChanged();
    }
    info->deleteLater();
  });
}

/*!
  \brief Destructor.
 */
Dsa::DynamicEntityLayerAlertTarget::~DynamicEntityLayerAlertTarget()
{
}

/*!
  \brief Returns the list of \l Esri::ArcGISRuntime::Geometry which are in the \a targetArea.

  \note No exact intersection tests are carried out to create this list.
 */
QList<Geometry> DynamicEntityLayerAlertTarget::targetGeometries(const Esri::ArcGISRuntime::Envelope &targetArea) const
{
  // if the quadtree has been built, use  it to return the set of candidate geometries
  if (m_quadtree) { return m_quadtree->candidateIntersections(targetArea); }

  // otherwise, return all of the geometry in the overlay
  QList<Geometry> geometries;
  for (auto dynamicEntity : m_entityGraphics)
  {
    if (!dynamicEntity) { continue; }
    geometries.append(dynamicEntity->geometry());
  }

  return geometries;
}

/*!
  \brief Returns an empty QVariant.
 */
QVariant DynamicEntityLayerAlertTarget::targetValue() const
{
  return QVariant{};
}

/*!
  \internal

  Connect signals etc. for a new \a dynamicEntity.
 */
void DynamicEntityLayerAlertTarget::connectEntityGraphic(Esri::ArcGISRuntime::DynamicEntity *dynamicEntity)
{
  // check for an existing graphic
  Graphic *graphic;
  if (m_entityGraphics.contains(dynamicEntity->entityId()))
  {
    // update the geometry
    graphic = m_entityGraphics[dynamicEntity->entityId()];
    graphic->setGeometry(dynamicEntity->geometry());
  }
  else
  {
      // if no graphic existed for the entity id, construct a new one and insert it into the graphics lookup
    graphic = new Graphic(dynamicEntity->geometry());
    connect(graphic, &Graphic::geometryChanged, this, &DynamicEntityLayerAlertTarget::dataChanged); // trigger the dataChanged signal on geometry updates
    m_entityGraphics[dynamicEntity->entityId()] = graphic;

    // if the quadtree has already been initialized, append the new graphic
    if (m_quadtree) { m_quadtree->appendGeoElment(graphic); }
    else
    {
      // otherwise call the rebuild method to initialize it
      rebuildQuadtree();
    }
    emit dataChanged();
  }
}


/*!
  \brief internal.

  Build the quadtree used to find intersections with entity geometry etc.
 */
void DynamicEntityLayerAlertTarget::rebuildQuadtree()
{
  // destroy the quadtree if it already existed
  if (m_quadtree)
  {
    delete m_quadtree;
    m_quadtree = nullptr;
  }

  // build a list of pointers to geoelements for every graphic in the lookup
  QList<GeoElement*> elements;
  for (auto const dynamicEntity : std::as_const(m_entityGraphics))
  {
    if (!dynamicEntity) { continue; }
    elements.append(dynamicEntity);
  }

  // if there is more than 1 element in the overlay, build a quadtree
  if (elements.size() > 1) { m_quadtree = new GeometryQuadtree(m_dynamicEntityLayer->fullExtent(), elements, 8, this); }
}

} // Dsa
