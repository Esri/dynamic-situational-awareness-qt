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

#include "Viewshed360.h"

// C++ API headers
#include "AnalysisOverlay.h"
#include "Viewshed.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::Viewshed360
  \inmodule Dsa
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
  \property Viewshed360::visible
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
  \property Viewshed360::name
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
  \property Viewshed360::minDistance
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
  \property Viewshed360::maxDistance
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
  \property Viewshed360::horizontalAngle
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
  \property Viewshed360::verticalAngle
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
  \property Viewshed360::offsetZ
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
  \property Viewshed360::headingEnabled
  \brief Returns whether heading is enabled for the viewshed.

  Heading will not be used if the viewshed is in 360 degree mode.
 */
bool Viewshed360::isHeadingEnabled() const
{
  return !m_is360Mode;
}

/*!
  \property Viewshed360::pitchEnabled
  \brief Returns whether pitch is enabled for the viewshed.

  Pitch will not be used if the viewshed is in 360 degree mode.
 */
bool Viewshed360::isPitchEnabled() const
{
  return !m_is360Mode;
}

/*!
  \property Viewshed360::offsetEnabled
  \brief Returns whether offset z is enabled for the viewshed.
 */
bool Viewshed360::isOffsetZEnabled() const
{
  return false;
}

/*!
  \property Viewshed360::is360Mode
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

// Signal Documentation
/*!
  \fn void Viewshed360::is360ModeChanged();
  \brief Signal emitted when the is360Mode property changes.
 */

/*!
  \fn void Viewshed360::offsetZEnabledChanged();
  \brief Signal emitted when the offsetZEnabled property changes.
 */

/*!
  \fn void Viewshed360::pitchEnabledChanged();
  \brief Signal emitted when the pitchEnabledChanged property changes.
 */

/*!
  \fn void Viewshed360::headingEnabledChanged();
  \brief Signal emitted when the headingEnabled property changes.
 */

/*!
  \fn void Viewshed360::offsetZChanged();
  \brief Signal emitted when the offsetZ property changes.
 */

/*!
  \fn void Viewshed360::pitchChanged();
  \brief Signal emitted when the pitch property changes.
 */

/*!
  \fn void Viewshed360::headingChanged();
  \brief Signal emitted when the heading property changes.
 */

/*!
  \fn void Viewshed360::verticalAngleChanged();
  \brief Signal emitted when the verticalAngle property changes.
 */

/*!
  \fn void Viewshed360::horizontalAngleChanged();
  \brief Signal emitted when the horizontalAngle property changes.
 */

/*!
  \fn void Viewshed360::maxDistanceChanged();
  \brief Signal emitted when the maxDistance property changes.
 */

/*!
  \fn void Viewshed360::minDistanceChanged();
  \brief Signal emitted when the minDistance property changes.
 */

/*!
  \fn void Viewshed360::nameChanged();
  \brief Signal emitted when the name property changes.
 */

/*!
  \fn void Viewshed360::visibleChanged();
  \brief Signal emitted when the visible property changes.
 */

