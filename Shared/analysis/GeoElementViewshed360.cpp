// Copyright 2018 ESRI
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

#include "GeoElementViewshed360.h"

// C++ API headers
#include "AnalysisOverlay.h"
#include "AttributeListModel.h"
#include "GeoElementViewshed.h"

// STL headers
#include <cmath>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

constexpr double c_defaultPitch = 0.0;
constexpr double c_defaultHorizontalAngle = 120.0;
constexpr double c_defaultVerticalAngle = 90.0;
constexpr double c_defaultMinDistance = 0.0;
constexpr double c_defaultMaxDistance = 500.0;
constexpr double c_defaultOffsetZ = 5.0;

/*!
  \class GeoElementViewshed360
  \inherits Viewshed360
  \brief This type represents a 360 degree viewshed centered upon a GeoElement

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
  m_geoElement(geoElement),
  m_headingAttribute(headingAttribute),
  m_pitchAttribute(pitchAttribute)
{
  update360Mode(is360Mode());
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
  return m_geoElement.data();
}

/*!
  \brief Returns the heading attribute of the \l Esri::ArcGISRuntime::GeoElement in degrees.

  If not set, this returns \c NAN.
 */
double GeoElementViewshed360::heading() const
{
  if (m_headingAttribute.isEmpty())
    return static_cast<GeoElementViewshed*>(viewshed())->headingOffset();

  if (m_geoElement.isNull())
    return NAN;

  return m_geoElement->attributes()->attributeValue(m_headingAttribute).toDouble();
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

    if (!m_viewsheds360Offsets.isEmpty())
    {
      double headingOffset1 = heading + c_defaultHorizontalAngle;
      constexpr double horizontalAngle360Value = 360.0;
      if (headingOffset1 > horizontalAngle360Value)
        headingOffset1 -= horizontalAngle360Value;

      static_cast<GeoElementViewshed*>(m_viewsheds360Offsets[0])->setHeadingOffset(headingOffset1);
      static_cast<GeoElementViewshed*>(m_viewsheds360Offsets[1])->setHeadingOffset(headingOffset1 + c_defaultHorizontalAngle);
    }
  }
  else
  {
    if (m_geoElement.isNull())
      return;

    auto attributes = m_geoElement->attributes();
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

  if (m_geoElement.isNull())
    return NAN;

  return m_geoElement->attributes()->attributeValue(m_pitchAttribute).toDouble();
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
    if (m_geoElement.isNull())
      return;

    auto attributes = m_geoElement->attributes();
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
  return static_cast<GeoElementViewshed*>(viewshed())->offsetZ();
}

/*!
  \brief Sets the offset z value to \a offsetZ meters.
 */
void GeoElementViewshed360::setOffsetZ(double offsetZ)
{
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
  \brief Returns whether offset z is enabled for the viewshed.

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

/*!
  \internal
 */
void GeoElementViewshed360::update360Mode(bool is360Mode)
{
  // update the viewshed to cover 360 degrees if is360Mode is true.
  auto overlay = analysisOverlay();

  // the 1st time the viewshed is set to be 360 degrees, the m_viewsheds360Offsets list is populated with
  // 2 additional viewsheds to cover the full range, since a single viewshed can only cover 120 degrees.
  if (is360Mode && m_viewsheds360Offsets.isEmpty() &&
      overlay && !m_geoElement.isNull())
  {
    double headingOffset1 = static_cast<GeoElementViewshed*>(viewshed())->headingOffset() + c_defaultHorizontalAngle;
    constexpr double horizontalAngle360Value = 360.0;
    if (headingOffset1 > horizontalAngle360Value)
      headingOffset1 -= horizontalAngle360Value;

    auto viewshedOffset1 = new GeoElementViewshed(m_geoElement.data(), c_defaultHorizontalAngle, c_defaultVerticalAngle,
                                                  minDistance(), maxDistance(), headingOffset1, 0.0, this);

    viewshedOffset1->setOffsetZ(c_defaultOffsetZ);
    viewshedOffset1->setVisible(isVisible());
    overlay->analyses()->append(viewshedOffset1);
    m_viewsheds360Offsets.append(viewshedOffset1);

    auto viewshedOffset2 = new GeoElementViewshed(m_geoElement.data(), c_defaultHorizontalAngle, c_defaultVerticalAngle,
                                                  minDistance(), maxDistance(), headingOffset1 + c_defaultHorizontalAngle, 0.0, this);

    viewshedOffset2->setOffsetZ(c_defaultOffsetZ);
    viewshedOffset2->setVisible(isVisible());
    overlay->analyses()->append(viewshedOffset2);
    m_viewsheds360Offsets.append(viewshedOffset2);
  }

  viewshed()->setHorizontalAngle(c_defaultHorizontalAngle);
  viewshed()->setVerticalAngle(c_defaultVerticalAngle);
  setPitch(c_defaultPitch);

  emit horizontalAngleChanged();
  emit verticalAngleChanged();

  // set the 2 offset viewsheds to be visible if we are in 360 degree mode.
  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setVisible(isVisible() && is360Mode);
  }

  emit headingEnabledChanged();
  emit pitchEnabledChanged();
}

} // Dsa
