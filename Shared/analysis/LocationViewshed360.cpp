
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

#include "LocationViewshed360.h"

// dsa app headers
#include "ViewshedController.h"

// C++ API headers
#include "AnalysisOverlay.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "LocationViewshed.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

constexpr double c_defaultHeading = 0.0;
constexpr double c_defaultPitch = 90.0;
constexpr double c_defaultHorizontalAngle = 360.0;
constexpr double c_defaultVerticalAngle = 90.0;
constexpr double c_defaultMinDistance = 0.0;
constexpr double c_defaultMaxDistance = 500.0;

/*!
  \class Dsa::LocationViewshed360
  \inmodule Dsa
  \inherits Viewshed360
  \brief A 360 degree viewshed centered upon the current Location.
  */

/*!
  \brief Constructor for a 360 degree viewshed centered upon the current Location.

  \list
    \li \a point - The \l Esri::ArcGISRuntime::Point which the viewshehd will be centered upon.
    \li \a graphicsOverlay - The \l Esri::ArcGISRuntime::GraphicsOverlay which will contains the viewshed direction graphic.
    \li \a analysisOverlay - The \l Esri::ArcGISRuntime::AnalysisOverlay which contains the viewshed.
    \li \a parent - An optional parent.
  \endlist
 */
LocationViewshed360::LocationViewshed360(const Point& point, GraphicsOverlay* graphicsOverlay, AnalysisOverlay* analysisOverlay, QObject* parent) :
  Viewshed360(new LocationViewshed(point, c_defaultHeading, c_defaultPitch, c_defaultHorizontalAngle,
                                   c_defaultVerticalAngle, c_defaultMinDistance, c_defaultMaxDistance, parent), analysisOverlay, parent),
  m_graphicsOverlay(graphicsOverlay)
{
  m_locationViewshedGraphic = new Graphic(point, parent);
  constexpr double headingOffset = -180.0;
  m_locationViewshedGraphic->attributes()->insertAttribute(ViewshedController::VIEWSHED_HEADING_ATTRIBUTE, headingOffset);
  m_locationViewshedGraphic->attributes()->insertAttribute(ViewshedController::VIEWSHED_PITCH_ATTRIBUTE, is360Mode() ? 180.0 : c_defaultPitch);
  m_graphicsOverlay->graphics()->append(m_locationViewshedGraphic);

  connect(this, &Viewshed360::is360ModeChanged, this, [this]()
  {
    const double newGraphicPitch = is360Mode() ? 180.0 : pitch();
    m_locationViewshedGraphic->attributes()->replaceAttribute(ViewshedController::VIEWSHED_PITCH_ATTRIBUTE, newGraphicPitch);
  });
}

/*!
  \brief Destructor.
 */
LocationViewshed360::~LocationViewshed360()
{
  if (!m_graphicsOverlay.isNull())
    m_graphicsOverlay->graphics()->removeOne(m_locationViewshedGraphic);
}

/*!
  \brief Returns the \l Esri::ArcGISRuntime::Point which the viewshed is centered upon.
 */
Point LocationViewshed360::point() const
{
  return static_cast<LocationViewshed*>(viewshed())->location();
}

/*!
  \brief Sets the \l Esri::ArcGISRuntime::Point which the viewshed is centered upon to \a point.
 */
void LocationViewshed360::setPoint(const Point& point)
{
  static_cast<LocationViewshed*>(viewshed())->setLocation(point);
  m_locationViewshedGraphic->setGeometry(point);
  setOffsetZ(offsetZ());
}

/*!
  \brief Sets the viewshed to be \a visible.
 */
void LocationViewshed360::setVisible(bool visible)
{
  Viewshed360::setVisible(visible);
  m_locationViewshedGraphic->setVisible(isVisible());
}

/*!
  \brief Returns the heading in degrees.
 */
double LocationViewshed360::heading() const
{
  return static_cast<LocationViewshed*>(viewshed())->heading();
}

/*!
  \brief Sets the heading to \a heading degrees.
 */
void LocationViewshed360::setHeading(double heading)
{
  if (static_cast<LocationViewshed*>(viewshed())->heading() == heading)
    return;

  static_cast<LocationViewshed*>(viewshed())->setHeading(heading);

  constexpr double headingOffset = -180.0;
  m_locationViewshedGraphic->attributes()->replaceAttribute(ViewshedController::VIEWSHED_HEADING_ATTRIBUTE, heading + headingOffset);
  emit headingChanged();
}

/*!
  \brief Returns the pitch in degrees.
 */
double LocationViewshed360::pitch() const
{
  return static_cast<LocationViewshed*>(viewshed())->pitch();
}

/*!
  \brief Sets the pitch to \a heading degrees.
 */
void LocationViewshed360::setPitch(double pitch)
{
  if (is360Mode())
    return;

  if (static_cast<LocationViewshed*>(viewshed())->pitch() == pitch)
    return;

  static_cast<LocationViewshed*>(viewshed())->setPitch(pitch);

  m_locationViewshedGraphic->attributes()->replaceAttribute(ViewshedController::VIEWSHED_PITCH_ATTRIBUTE, pitch);

  emit pitchChanged();
}

double LocationViewshed360::offsetZ() const
{
  return m_offsetZ;
}

void LocationViewshed360::setOffsetZ(double offset)
{
  if (!m_locationViewshedGraphic)
    return;

  m_offsetZ = offset;

  Point p = geometry_cast<Point>(m_locationViewshedGraphic->geometry());

  double newZ = p.z() + offset;

  static_cast<LocationViewshed*>(viewshed())->setLocation(Point(p.x(), p.y(), newZ));

  emit offsetZChanged();
}

} // Dsa
