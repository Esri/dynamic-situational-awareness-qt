
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

#include "GeoElementUtils.h"

// C++ API headers
#include "GeoElement.h"
#include "Feature.h"
#include "Graphic.h"
#include "EncFeature.h"
#include "WmsFeature.h"
#include "KmlPlacemark.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::GeoElementSignaler
  \brief A lightweight class that wraps a GeoElement and provides the geometryChanged signal.

  This object does not take ownership of the GeoElement. You can choose to do that
  by passing in the managed geoElement as the parent.

  \sa Dsa::GeoElementUtils::toQObject
 */

/*!
  \brief Constructor that takes a \a geoElement and an optional \a parent.
 */
GeoElementSignaler::GeoElementSignaler(GeoElement* geoElement, QObject* parent) :
  QObject(parent),
  m_geoElement(geoElement)
{
  if (dynamic_cast<Feature*>(m_geoElement))
  {
    connect(static_cast<Feature*>(m_geoElement), &Feature::geometryChanged,
            this, &GeoElementSignaler::geometryChanged);
  }
  else if (dynamic_cast<Graphic*>(m_geoElement))
  {
    connect(static_cast<Graphic*>(m_geoElement), &Graphic::geometryChanged,
            this, &GeoElementSignaler::geometryChanged);
  }
  else if (dynamic_cast<KmlPlacemark*>(m_geoElement))
  {
    connect(static_cast<KmlPlacemark*>(m_geoElement), &KmlPlacemark::geometryChanged,
            this, &GeoElementSignaler::geometryChanged);
  }
  else if (dynamic_cast<EncFeature*>(m_geoElement))
  {
    connect(static_cast<EncFeature*>(m_geoElement), &EncFeature::geometryChanged,
            this, &GeoElementSignaler::geometryChanged);
  }
  else if (dynamic_cast<WmsFeature*>(m_geoElement))
  {
    connect(static_cast<WmsFeature*>(m_geoElement), &WmsFeature::geometryChanged,
            this, &GeoElementSignaler::geometryChanged);
  }
}

/*!
  \brief Destructor.
 */
GeoElementSignaler::~GeoElementSignaler() = default;

/*!
  \fn void Dsa::GeoElementUtils::setParent(const QList<Esri::ArcGISRuntime::GeoElement*> geoElements, QObject* parent)
  \brief Allows the \a parent to be applied to all the \a geoElements.
 */
void GeoElementUtils::setParent(const QList<Esri::ArcGISRuntime::GeoElement*> geoElements, QObject* parent)
{
  if (geoElements.isEmpty())
    return;

  for (auto* geoElement : geoElements)
  {
    toQObject(geoElement)->setParent(parent);
  }
}

/*!
  \fn void Dsa::GeoElementUtils::setParent(Esri::ArcGISRuntime::GeoElement* geoElement, QObject* parent)
  \brief Allows the \a parent to be applied to \a geoElement.
 */
void GeoElementUtils::setParent(Esri::ArcGISRuntime::GeoElement* geoElement, QObject* parent)
{
  if (geoElement)
  {
    toQObject(geoElement)->setParent(parent);
  }
}

/*!
  \fn QObject* Dsa::GeoElementUtils::toQObject(Esri::ArcGISRuntime::GeoElement* geoElement)
  \brief Returns a QObject* from a GeoElement.
 */
QObject* GeoElementUtils::toQObject(Esri::ArcGISRuntime::GeoElement* geoElement)
{
  if (dynamic_cast<Feature*>(geoElement))
    return static_cast<Feature*>(geoElement);

  if (dynamic_cast<Graphic*>(geoElement))
    return static_cast<Graphic*>(geoElement);

  if (dynamic_cast<KmlPlacemark*>(geoElement))
    return static_cast<KmlPlacemark*>(geoElement);

  if (dynamic_cast<EncFeature*>(geoElement))
    return static_cast<EncFeature*>(geoElement);

  if (dynamic_cast<WmsFeature*>(geoElement))
    return static_cast<WmsFeature*>(geoElement);

  return nullptr;
}

} // Dsa
