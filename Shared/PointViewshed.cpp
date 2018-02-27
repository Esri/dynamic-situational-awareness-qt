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

#include "PointViewshed.h"
#include "LocationViewshed.h"
#include "Point.h"
#include "AnalysisOverlay.h"
#include "SimpleMarkerSceneSymbol.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"

using namespace Esri::ArcGISRuntime;

PointViewshed::PointViewshed(const Point& point, GraphicsOverlay* graphicsOverlay, AnalysisOverlay* analysisOverlay, QObject* parent) :
  AbstractViewshed(new LocationViewshed(point, 0.0, 90.0, 120.0, 90.0, 5.0, 1000.0, parent), analysisOverlay, parent),
  m_graphicsOverlay(graphicsOverlay)
{
  SimpleMarkerSceneSymbol* smss = SimpleMarkerSceneSymbol::cone(QColor("red"), 0.5, 70, parent);
  smss->setWidth(12);
  m_locationViewshedGraphic = new Graphic(point, smss, parent);
  m_graphicsOverlay->graphics()->append(m_locationViewshedGraphic);

  smss->setHeading(-180.0);
  smss->setPitch(90.0);
}

PointViewshed::~PointViewshed()
{
  m_graphicsOverlay->graphics()->removeOne(m_locationViewshedGraphic);
}

Point PointViewshed::point() const
{
  return static_cast<LocationViewshed*>(m_viewshed)->location();
}

void PointViewshed::setPoint(const Point& point)
{
  static_cast<LocationViewshed*>(m_viewshed)->setLocation(point);
  m_locationViewshedGraphic->setGeometry(point);

  for (auto viewshed : m_viewsheds360Offsets)
  {
    static_cast<LocationViewshed*>(viewshed)->setLocation(point);
  }
}

void PointViewshed::setVisible(bool visible)
{
  AbstractViewshed::setVisible(visible);
  m_locationViewshedGraphic->setVisible(isVisible());
}

double PointViewshed::heading() const
{
  return static_cast<LocationViewshed*>(m_viewshed)->heading();
}

void PointViewshed::setHeading(double heading)
{
  if (static_cast<LocationViewshed*>(m_viewshed)->heading() == heading)
    return;

  static_cast<LocationViewshed*>(m_viewshed)->setHeading(heading);

  SimpleMarkerSceneSymbol* smss = static_cast<SimpleMarkerSceneSymbol*>(m_locationViewshedGraphic->symbol());
  smss->setHeading(heading - 180.0);

  if (!m_viewsheds360Offsets.isEmpty())
  {
    double headingOffset1 = heading + 120.0;
    if (headingOffset1 > 360)
      headingOffset1 -= 360;

    static_cast<LocationViewshed*>(m_viewsheds360Offsets[0])->setHeading(headingOffset1);
    static_cast<LocationViewshed*>(m_viewsheds360Offsets[1])->setHeading(headingOffset1 + 120.0);
  }

  emit headingChanged();
}

double PointViewshed::pitch() const
{
  return static_cast<LocationViewshed*>(m_viewshed)->pitch();
}

void PointViewshed::setPitch(double pitch)
{
  if (is360Mode())
    return;

  if (static_cast<LocationViewshed*>(m_viewshed)->pitch() == pitch)
    return;

  static_cast<LocationViewshed*>(m_viewshed)->setPitch(pitch);

  SimpleMarkerSceneSymbol* smss = static_cast<SimpleMarkerSceneSymbol*>(m_locationViewshedGraphic->symbol());
  smss->setPitch(pitch);

  emit pitchChanged();
}

AbstractViewshed::AnalysisType PointViewshed::analysisType() const
{
  return AbstractViewshed::AnalysisType::PointViewshed;
}

void PointViewshed::update360Mode(bool is360Mode)
{
  if (is360Mode && m_viewsheds360Offsets.isEmpty())
  {
    double headingOffset1 = heading() + 120.0;
    if (headingOffset1 > 360)
      headingOffset1 -= 360;
    auto viewshedOffset1 = new LocationViewshed(point(), headingOffset1, pitch(), 120.0, 90.0,
                                                minDistance(), maxDistance(), this);

    viewshedOffset1->setVisible(isVisible());
    m_analysisOverlay->analyses()->append(viewshedOffset1);
    m_viewsheds360Offsets.append(viewshedOffset1);

    auto viewshedOffset2 = new LocationViewshed(point(), headingOffset1 + 120.0, pitch(), 120.0, 90.0,
                                                minDistance(), maxDistance(), this);

    viewshedOffset2->setVisible(isVisible());
    m_analysisOverlay->analyses()->append(viewshedOffset2);
    m_viewsheds360Offsets.append(viewshedOffset2);
  }

  m_viewshed->setHorizontalAngle(120.0);
  m_viewshed->setVerticalAngle(90.0);
  static_cast<LocationViewshed*>(m_viewshed)->setPitch(90.0);

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
