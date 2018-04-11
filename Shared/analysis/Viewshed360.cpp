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

#include "Viewshed360.h"

// C++ API headers
#include "AnalysisOverlay.h"
#include "Viewshed.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Viewshed360
  \inherits QObject
  \brief A viewshed which can cover up to 360 degrees.

  When in 360 degree mode the primary viewshed will be supplemented by 2 additional
  viewsheds to complete the full arc, since a single viewshed can only cover 120 degrees.

  \sa Esri::ArcGISRuntime::Viewshed
  */

/*!
  \brief Constructor for a 360 degree viewshed.

  \list
    \li \a viewshed - The primary \l Esri::ArcGISRuntime::Viewshed. When in 360 degree mode this will be supplemented
        by 2 additional viewsheds to complete the full arc, since a single viewshed can only cover 120 degrees.
    \li \a analysisOverlay - The \l Esri::ArcGISRuntime::AnalysisOverlay which contains the viewshed(s).
    \li \a parent - An optional parent.
  \endlist
 */
Viewshed360::Viewshed360(Viewshed* viewshed, AnalysisOverlay* analysisOverlay, QObject* parent) :
  QObject(parent),
  m_viewshed(viewshed),
  m_analysisOverlay(analysisOverlay)
{
}

/*!
  \brief Destructor.
 */
Viewshed360::~Viewshed360()
{
}

/*!
  \brief Removes the viewshed from the \l Esri::ArcGISRuntime::AnalysisOverlay.
 */
void Viewshed360::removeFromOverlay()
{
  if (m_analysisOverlay.isNull())
    return;

  for (auto viewshed : m_viewsheds360Offsets)
  {
    m_analysisOverlay->analyses()->removeOne(viewshed);
  }

  m_analysisOverlay->analyses()->removeOne(m_viewshed);
}

/*!
  \brief Returns whether the viewshed is visible.
 */
bool Viewshed360::isVisible() const
{
  return m_viewshed->isVisible();
}

/*!
  \brief Sets whether the viewshed is visible to \a visible.
 */
void Viewshed360::setVisible(bool visible)
{
  if (m_viewshed->isVisible() == visible)
    return;

  m_viewshed->setVisible(visible);

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setVisible(visible && m_is360Mode);
  }

  emit visibleChanged();
}

/*!
  \brief Returns the name of the viewshed.
 */
QString Viewshed360::name() const
{
  return m_name;
}

/*!
  \brief Sets the name of the viewshed to \a name.
 */
void Viewshed360::setName(const QString& name)
{
  if (m_name == name)
    return;

  m_name = name;

  emit nameChanged();
}

/*!
  \brief Returns the minimum distance in meters from the observer at which visibility will be evaluated.
 */
double Viewshed360::minDistance() const
{
  return viewshed()->minDistance();
}

/*!
  \brief Sets the minimum distance in meters from the observer at which visibility will be evaluated to \a minDistance meters.
 */
void Viewshed360::setMinDistance(double minDistance)
{
  if (viewshed()->minDistance() == minDistance)
    return;

  viewshed()->setMinDistance(minDistance);

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setMinDistance(minDistance);
  }

  emit minDistanceChanged();
}

/*!
  \brief Returns the maximum distance in meters from the observer at which visibility will be evaluated.
 */
double Viewshed360::maxDistance() const
{
  return viewshed()->maxDistance();
}

/*!
  \brief Sets the maximum distance in meters from the observer at which visibility will be evaluated to \a maxDistance meters.
 */
void Viewshed360::setMaxDistance(double maxDistance)
{
  if (viewshed()->maxDistance() == maxDistance)
    return;

  viewshed()->setMaxDistance(maxDistance);

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setMaxDistance(maxDistance);
  }

  emit maxDistanceChanged();
}

/*!
  \brief Returns the horizontal angle in degrees of the observer's field of vision.
 */
double Viewshed360::horizontalAngle() const
{
  if (m_is360Mode)
  {
    constexpr double horizontalAngle360Mode = 360.0;
    return horizontalAngle360Mode; // 360 mode
  }

  return viewshed()->horizontalAngle();
}

/*!
  \brief Sets the horizontal angle in degrees of the observer's field of vision to \a horizontalAngle.

  \note If the viewshed is set to be 360 degrees, this is ignored.
 */
void Viewshed360::setHorizontalAngle(double horizontalAngle)
{
  if (m_is360Mode)
    return;

  if (viewshed()->horizontalAngle() == horizontalAngle)
    return;

  viewshed()->setHorizontalAngle(horizontalAngle);

  emit horizontalAngleChanged();
}

/*!
  \brief Returns the vertical angle in degrees of the observer's field of vision.
 */
double Viewshed360::verticalAngle() const
{
  if (m_is360Mode)
  {
    constexpr double verticalAngle360Mode = 90.0;
    return verticalAngle360Mode; // 360 mode
  }

  return viewshed()->verticalAngle();
}

/*!
  \brief Sets the vertical angle in degrees of the observer's field of vision to \a verticalAngle.

  \note If the viewshed is set to be 360 degrees, this is ignored.
 */
void Viewshed360::setVerticalAngle(double verticalAngle)
{
  if (m_is360Mode)
    return;

  if (viewshed()->verticalAngle() == verticalAngle)
    return;

  viewshed()->setVerticalAngle(verticalAngle);

  emit verticalAngleChanged();
}

/*!
  \brief Returns the offset Z value of the viewshed in meters.

  The default value is \c 0.0.
 */
double Viewshed360::offsetZ() const
{
  constexpr double offsetZDefault = 0.0;
  return offsetZDefault;
}

/*!
  \brief Sets the offset z value of the viewshed in meters.

  This method must be overriden to have any effect.
 */
void Viewshed360::setOffsetZ(double)
{
}

/*!
  \brief Returns whether heading is enabled for the viewshed.

  Heading will not be used if the viewshed is in 360 degree mode.
 */
bool Viewshed360::isHeadingEnabled() const
{
  return !m_is360Mode;
}

/*!
  \brief Returns whether pitch is enabled for the viewshed.

  Pitch will not be used if the viewshed is in 360 degree mode.
 */
bool Viewshed360::isPitchEnabled() const
{
  return !m_is360Mode;
}

/*!
  \brief Returns whether offset z is enabled for the viewshed.
 */
bool Viewshed360::isOffsetZEnabled() const
{
  return false;
}

/*!
  \brief Returns whether the viewshed is in 360 degree mode.
 */
bool Viewshed360::is360Mode() const
{
  return m_is360Mode;
}

/*!
  \brief Sets whether the viewshed is in 360 degree mode to \a is360Mode.

  When in 360 degree mode the primary viewshed will be supplemented by 2 additional
  viewsheds to complete the full arc, since a single viewshed can only cover 120 degrees.
 */
void Viewshed360::set360Mode(bool is360Mode)
{
  if (m_is360Mode == is360Mode)
    return;

  m_is360Mode = is360Mode;

  update360Mode(m_is360Mode);

  emit is360ModeChanged();
}

/*!
  \brief Returns the primary \l Esri::ArcGISRuntime::Viewshed.
 */
Viewshed* Viewshed360::viewshed() const
{
  return m_viewshed.data();
}

/*!
  \brief Returns the \l Esri::ArcGISRuntime::AnalysisOverlay which contains the viewshed.
 */
AnalysisOverlay* Viewshed360::analysisOverlay() const
{
  return m_analysisOverlay.data();
}

} // Dsa
