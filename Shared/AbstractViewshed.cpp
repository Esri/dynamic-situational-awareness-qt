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
  QObject(parent),
  m_viewshed(viewshed),
  m_analysisOverlay(analysisOverlay)
{
}

AbstractViewshed::~AbstractViewshed()
{
  for (auto viewshed : m_viewsheds360Offsets)
  {
    m_analysisOverlay->analyses()->removeOne(viewshed);
  }
}

bool AbstractViewshed::isVisible() const
{
  return m_viewshed->isVisible();
}

void AbstractViewshed::setVisible(bool visible)
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

QString AbstractViewshed::name() const
{
  return m_name;
}

void AbstractViewshed::setName(const QString& name)
{
  if (m_name == name)
    return;

  m_name = name;

  emit nameChanged();
}

double AbstractViewshed::minDistance() const
{
  return m_viewshed->minDistance();
}

void AbstractViewshed::setMinDistance(double minDistance)
{
  if (m_viewshed->minDistance() == minDistance)
    return;

  m_viewshed->setMinDistance(minDistance);

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setMinDistance(minDistance);
  }

  emit minDistanceChanged();
}

double AbstractViewshed::maxDistance() const
{
  return m_viewshed->maxDistance();
}

void AbstractViewshed::setMaxDistance(double maxDistance)
{
  if (m_viewshed->maxDistance() == maxDistance)
    return;

  m_viewshed->setMaxDistance(maxDistance);

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

  return m_viewshed->horizontalAngle();
}

void AbstractViewshed::setHorizontalAngle(double horizontalAngle)
{
  if (m_is360Mode)
    return;

  if (m_viewshed->horizontalAngle() == horizontalAngle)
    return;

  m_viewshed->setHorizontalAngle(horizontalAngle);

  emit horizontalAngleChanged();
}

double AbstractViewshed::verticalAngle() const
{
  if (m_is360Mode)
    return 90.0; // 360 mode

  return m_viewshed->verticalAngle();
}

void AbstractViewshed::setVerticalAngle(double verticalAngle)
{
  if (m_is360Mode)
    return;

  if (m_viewshed->verticalAngle() == verticalAngle)
    return;

  m_viewshed->setVerticalAngle(verticalAngle);

  emit verticalAngleChanged();
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

Viewshed* AbstractViewshed::viewshed() const
{
  return m_viewshed;
}

AnalysisOverlay* AbstractViewshed::anlysisOverlay() const
{
  return m_analysisOverlay;
}
