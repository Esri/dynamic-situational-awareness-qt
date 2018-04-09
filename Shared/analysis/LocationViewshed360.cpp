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

#include "LocationViewshed360.h"

// example app headers
#include "ViewshedController.h"

// C++ API headers
#include "AnalysisOverlay.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "LocationViewshed.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {
namespace Analysis {

constexpr double c_defaultHeading = 0.0;
constexpr double c_defaultPitch = 90.0;
constexpr double c_defaultHorizontalAngle = 120.0;
constexpr double c_defaultVerticalAngle = 90.0;
constexpr double c_defaultMinDistance = 0.0;
constexpr double c_defaultMaxDistance = 500.0;

LocationViewshed360::LocationViewshed360(const Point& point, GraphicsOverlay* graphicsOverlay, AnalysisOverlay* analysisOverlay, QObject* parent) :
  Viewshed360(new LocationViewshed(point, c_defaultHeading, c_defaultPitch, c_defaultHorizontalAngle,
                                   c_defaultVerticalAngle, c_defaultMinDistance, c_defaultMaxDistance, parent), analysisOverlay, parent),
  m_graphicsOverlay(graphicsOverlay)
{
  m_locationViewshedGraphic = new Graphic(point, parent);
  constexpr double headingOffset = -180.0;
  m_locationViewshedGraphic->attributes()->insertAttribute(ViewshedController::VIEWSHED_HEADING_ATTRIBUTE, headingOffset);
  m_locationViewshedGraphic->attributes()->insertAttribute(ViewshedController::VIEWSHED_PITCH_ATTRIBUTE, c_defaultPitch);
  m_graphicsOverlay->graphics()->append(m_locationViewshedGraphic);

  update360Mode(is360Mode());
}

LocationViewshed360::~LocationViewshed360()
{
  if (!m_graphicsOverlay.isNull())
    m_graphicsOverlay->graphics()->removeOne(m_locationViewshedGraphic);
}

Point LocationViewshed360::point() const
{
  return static_cast<LocationViewshed*>(viewshed())->location();
}

void LocationViewshed360::setPoint(const Point& point)
{
  static_cast<LocationViewshed*>(viewshed())->setLocation(point);
  m_locationViewshedGraphic->setGeometry(point);

  for (auto viewshed : m_viewsheds360Offsets)
  {
    static_cast<LocationViewshed*>(viewshed)->setLocation(point);
  }
}

void LocationViewshed360::setVisible(bool visible)
{
  Viewshed360::setVisible(visible);
  m_locationViewshedGraphic->setVisible(isVisible());
}

double LocationViewshed360::heading() const
{
  return static_cast<LocationViewshed*>(viewshed())->heading();
}

void LocationViewshed360::setHeading(double heading)
{
  if (static_cast<LocationViewshed*>(viewshed())->heading() == heading)
    return;

  static_cast<LocationViewshed*>(viewshed())->setHeading(heading);

  constexpr double headingOffset = -180.0;
  m_locationViewshedGraphic->attributes()->replaceAttribute(ViewshedController::VIEWSHED_HEADING_ATTRIBUTE, heading + headingOffset);

  if (!m_viewsheds360Offsets.isEmpty())
  {
    constexpr double horizontalAngle360Value = 360.0;
    double headingOffset1 = heading + c_defaultHorizontalAngle;
    if (headingOffset1 > horizontalAngle360Value)
      headingOffset1 -= horizontalAngle360Value;

    static_cast<LocationViewshed*>(m_viewsheds360Offsets[0])->setHeading(headingOffset1);
    static_cast<LocationViewshed*>(m_viewsheds360Offsets[1])->setHeading(headingOffset1 + c_defaultHorizontalAngle);
  }

  emit headingChanged();
}

double LocationViewshed360::pitch() const
{
  return static_cast<LocationViewshed*>(viewshed())->pitch();
}

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

void LocationViewshed360::update360Mode(bool is360Mode)
{
  auto overlay = analysisOverlay();

  if (!is360Mode)
    m_locationViewshedGraphic->attributes()->replaceAttribute(ViewshedController::VIEWSHED_PITCH_ATTRIBUTE, pitch());
  else
    m_locationViewshedGraphic->attributes()->replaceAttribute(ViewshedController::VIEWSHED_PITCH_ATTRIBUTE, 180.0);

  if (is360Mode && m_viewsheds360Offsets.isEmpty() && overlay)
  {
    double headingOffset1 = heading() + c_defaultHorizontalAngle;
    constexpr double horizontalAngle360Value = 360.0;
    if (headingOffset1 > horizontalAngle360Value)
      headingOffset1 -= horizontalAngle360Value;
    auto viewshedOffset1 = new LocationViewshed(point(), headingOffset1, pitch(), c_defaultHorizontalAngle, c_defaultVerticalAngle,
                                                minDistance(), maxDistance(), this);

    viewshedOffset1->setVisible(isVisible());
    overlay->analyses()->append(viewshedOffset1);
    m_viewsheds360Offsets.append(viewshedOffset1);

    auto viewshedOffset2 = new LocationViewshed(point(), headingOffset1 + c_defaultHorizontalAngle, pitch(), c_defaultHorizontalAngle,
                                                c_defaultVerticalAngle, minDistance(), maxDistance(), this);

    viewshedOffset2->setVisible(isVisible());
    overlay->analyses()->append(viewshedOffset2);
    m_viewsheds360Offsets.append(viewshedOffset2);
  }

  viewshed()->setHorizontalAngle(c_defaultHorizontalAngle);
  viewshed()->setVerticalAngle(c_defaultVerticalAngle);
  static_cast<LocationViewshed*>(viewshed())->setPitch(c_defaultPitch);

  emit horizontalAngleChanged();
  emit verticalAngleChanged();
  emit pitchChanged();

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setVisible(isVisible() && is360Mode);
  }

  emit headingEnabledChanged();
  emit pitchEnabledChanged();
}

} // Analysis
} // Dsa
