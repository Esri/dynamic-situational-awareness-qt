
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

#include "GeoElementViewshed360.h"

#include "GeoElementUtils.h"

// C++ API headers
#include "AnalysisOverlay.h"
#include "AttributeListModel.h"
#include "GeoElementViewshed.h"

// STL headers
#include <cmath>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

constexpr double c_defaultHorizontalAngle = 360.0;
constexpr double c_defaultVerticalAngle = 90.0;
constexpr double c_defaultMinDistance = 0.0;
constexpr double c_defaultMaxDistance = 500.0;

/*!
  \class Dsa::GeoElementViewshed360
  \inmodule Dsa
  \inherits Viewshed360
  \brief A 360 degree viewshed centered upon a GeoElement.

  \sa Esri::ArcGISRuntime::GeoElement
  */

/*!
  \brief Constructor for a 360 degree viewshed centered upon a GeoElement.

  \list
    \li \a geoElement - The \l Esri::ArcGISRuntime::GeoElement which the viewshed will be centered upon.
    \li \a analysisOverlay - The \l Esri::ArcGISRuntime::AnalysisOverlay which contains the viewshed.
    \li \a headingAttribute - the name of the heading attribute.
    \li \a pitchAttribute - the name of the pitch attribute.
    \li \a parent - An optional parent.
  \endlist
 */
GeoElementViewshed360::GeoElementViewshed360(GeoElement* geoElement, AnalysisOverlay* analysisOverlay,
                                             const QString& headingAttribute, const QString& pitchAttribute, QObject* parent) :
  Viewshed360(new GeoElementViewshed(geoElement, c_defaultHorizontalAngle, c_defaultVerticalAngle, c_defaultMinDistance, c_defaultMaxDistance, 0.0, 0.0, parent), analysisOverlay, parent),
  m_geoElementSignaler(new GeoElementSignaler(geoElement, GeoElementUtils::toQObject(geoElement))),
  m_headingAttribute(headingAttribute),
  m_pitchAttribute(pitchAttribute)
{
}

/*!
  \brief Destructor.
 */
GeoElementViewshed360::~GeoElementViewshed360()
{
}

/*!
  \brief Returns the \l Esri::ArcGISRuntime::GeoElement which the viewshed is centered upon.
 */
GeoElement* GeoElementViewshed360::geoElement() const
{
  return m_geoElementSignaler.isNull() ? nullptr : m_geoElementSignaler.data()->geoElement();
}

/*!
  \brief Returns the heading attribute of the \l Esri::ArcGISRuntime::GeoElement in degrees.

  If not set, this returns \c NAN.
 */
double GeoElementViewshed360::heading() const
{
  if (m_headingAttribute.isEmpty())
    return static_cast<GeoElementViewshed*>(viewshed())->headingOffset();

  if (m_geoElementSignaler.isNull())
    return NAN;

  return m_geoElementSignaler->geoElement()->attributes()->attributeValue(m_headingAttribute).toDouble();
}

/*!
  \brief Sets the heading attribute of the \l Esri::ArcGISRuntime::GeoElement to \a heading.

  The supplied value should be in degrees.
 */
void GeoElementViewshed360::setHeading(double heading)
{
  if (m_headingAttribute.isEmpty())
  {
    if (static_cast<GeoElementViewshed*>(viewshed())->headingOffset() == heading)
      return;

    static_cast<GeoElementViewshed*>(viewshed())->setHeadingOffset(heading);
  }
  else
  {
    if (m_geoElementSignaler.isNull())
      return;

    auto attributes = m_geoElementSignaler->geoElement()->attributes();
    if (attributes->attributeValue(m_headingAttribute).toDouble() == heading)
      return;

    if (attributes->containsAttribute(m_headingAttribute))
      attributes->replaceAttribute(m_headingAttribute, heading);
    else
      attributes->insertAttribute(m_headingAttribute, heading);
  }

  emit headingChanged();
}

/*!
  \brief Returns the pitch attribute of the \l Esri::ArcGISRuntime::GeoElement in degrees.

  If not set, this returns \c NAN.
 */
double GeoElementViewshed360::pitch() const
{
  if (m_pitchAttribute.isEmpty())
    return static_cast<GeoElementViewshed*>(viewshed())->pitchOffset();

  if (m_geoElementSignaler.isNull())
    return NAN;

  return m_geoElementSignaler->geoElement()->attributes()->attributeValue(m_pitchAttribute).toDouble();
}

/*!
  \brief Sets the pitch attribute of the \l Esri::ArcGISRuntime::GeoElement to \a pitch.

  The supplied value should be in degrees.
 */
void GeoElementViewshed360::setPitch(double pitch)
{
  if (m_pitchAttribute.isEmpty())
  {
    if (static_cast<GeoElementViewshed*>(viewshed())->pitchOffset() == pitch)
      return;

    static_cast<GeoElementViewshed*>(viewshed())->setPitchOffset(pitch);
  }
  else
  {
    if (m_geoElementSignaler.isNull())
      return;

    auto attributes = m_geoElementSignaler->geoElement()->attributes();
    if (attributes->attributeValue(m_pitchAttribute).toDouble() == pitch)
      return;

    if (attributes->containsAttribute(m_pitchAttribute))
      attributes->replaceAttribute(m_pitchAttribute, pitch);
    else
      attributes->insertAttribute(m_pitchAttribute, pitch);
  }

  emit pitchChanged();
}

/*!
  \brief Returns the offset Z value in meters.
 */
double GeoElementViewshed360::offsetZ() const
{
  return m_offsetZ;
}

/*!
  \brief Sets the offset Z value to \a offsetZ meters.
 */
void GeoElementViewshed360::setOffsetZ(double offsetZ)
{
  m_offsetZ = offsetZ;
  if (static_cast<GeoElementViewshed*>(viewshed())->offsetZ() == offsetZ)
    return;

  static_cast<GeoElementViewshed*>(viewshed())->setOffsetZ(offsetZ);

  emit offsetZChanged();
}

/*!
  \brief Returns whether heading is enabled for the viewshed.

  \note If the viewshed is currently in 360 degree mode, heading will not be used.
 */
bool GeoElementViewshed360::isHeadingEnabled() const
{
  return !is360Mode() && m_headingAttribute.isEmpty();
}

/*!
  \brief Returns whether pitch is enabled for the viewshed.

  \note If the viewshed is currently in 360 degree mode, pitch will not be used.
 */
bool GeoElementViewshed360::isPitchEnabled() const
{
  return !is360Mode() && m_pitchAttribute.isEmpty();
}

/*!
  \brief Returns whether offset Z is enabled for the viewshed.

  \note For this type, the value is always \c true.
 */
bool GeoElementViewshed360::isOffsetZEnabled() const
{
  return true;
}

/*!
  \brief Returns the attribute used for heading.
 */
QString GeoElementViewshed360::headingAttribute() const
{
  return m_headingAttribute;
}

/*!
  \brief Returns the attribute used for pitch.
 */
QString GeoElementViewshed360::pitchAttribute() const
{
  return m_pitchAttribute;
}

} // Dsa
