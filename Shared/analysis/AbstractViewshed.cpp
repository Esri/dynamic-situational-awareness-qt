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

#include "AbstractViewshed.h"
#include "Viewshed.h"
#include "AnalysisOverlay.h"

using namespace Esri::ArcGISRuntime;

AbstractViewshed::AbstractViewshed(Viewshed* viewshed, AnalysisOverlay* analysisOverlay, QObject* parent) :
  AbstractAnalysis(viewshed, analysisOverlay, parent)
{
}

AbstractViewshed::~AbstractViewshed()
{
}

void AbstractViewshed::removeFromOverlay()
{
  if (!m_analysisOverlay.isNull())
  {
    for (auto viewshed : m_viewsheds360Offsets)
    {
      m_analysisOverlay->analyses()->removeOne(viewshed);
    }
  }

  AbstractAnalysis::removeFromOverlay();
}

void AbstractViewshed::setVisible(bool visible)
{
  if (m_analysis->isVisible() == visible)
    return;

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setVisible(visible && m_is360Mode);
  }

  AbstractAnalysis::setVisible(visible);
}

double AbstractViewshed::minDistance() const
{
  return viewshed()->minDistance();
}

void AbstractViewshed::setMinDistance(double minDistance)
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

double AbstractViewshed::maxDistance() const
{
  return viewshed()->maxDistance();
}

void AbstractViewshed::setMaxDistance(double maxDistance)
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

double AbstractViewshed::horizontalAngle() const
{
  if (m_is360Mode)
    return 360.0; // 360 mode

  return viewshed()->horizontalAngle();
}

void AbstractViewshed::setHorizontalAngle(double horizontalAngle)
{
  if (m_is360Mode)
    return;

  if (viewshed()->horizontalAngle() == horizontalAngle)
    return;

  viewshed()->setHorizontalAngle(horizontalAngle);

  emit horizontalAngleChanged();
}

double AbstractViewshed::verticalAngle() const
{
  if (m_is360Mode)
    return 90.0; // 360 mode

  return viewshed()->verticalAngle();
}

void AbstractViewshed::setVerticalAngle(double verticalAngle)
{
  if (m_is360Mode)
    return;

  if (viewshed()->verticalAngle() == verticalAngle)
    return;

  viewshed()->setVerticalAngle(verticalAngle);

  emit verticalAngleChanged();
}

bool AbstractViewshed::isHeadingEnabled() const
{
  return !m_is360Mode;
}

bool AbstractViewshed::isPitchEnabled() const
{
  return !m_is360Mode;
}

bool AbstractViewshed::is360Mode() const
{
  return m_is360Mode;
}

void AbstractViewshed::set360Mode(bool is360Mode)
{
  if (m_is360Mode == is360Mode)
    return;

  m_is360Mode = is360Mode;

  update360Mode(m_is360Mode);

  emit is360ModeChanged();
}

double AbstractViewshed::offsetZ() const
{
  return 0.0;
}

void AbstractViewshed::setOffsetZ(double)
{
}

Viewshed* AbstractViewshed::viewshed() const
{
  return static_cast<Viewshed*>(m_analysis);
}
