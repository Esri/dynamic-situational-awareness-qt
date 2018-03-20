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

#include "pch.hpp"

#include "Viewshed360.h"
#include "Viewshed.h"
#include "AnalysisOverlay.h"

using namespace Esri::ArcGISRuntime;

Viewshed360::Viewshed360(Viewshed* viewshed, AnalysisOverlay* analysisOverlay, QObject* parent) :
  QObject(parent),
  m_viewshed(viewshed),
  m_analysisOverlay(analysisOverlay)
{
}

Viewshed360::~Viewshed360()
{
}

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

bool Viewshed360::isVisible() const
{
  return m_viewshed->isVisible();
}

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

QString Viewshed360::name() const
{
  return m_name;
}

void Viewshed360::setName(const QString& name)
{
  if (m_name == name)
    return;

  m_name = name;

  emit nameChanged();
}

double Viewshed360::minDistance() const
{
  return viewshed()->minDistance();
}

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

double Viewshed360::maxDistance() const
{
  return viewshed()->maxDistance();
}

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

double Viewshed360::horizontalAngle() const
{
  if (m_is360Mode)
  {
    constexpr double horizontalAngle360Mode = 360.0;
    return horizontalAngle360Mode; // 360 mode
  }

  return viewshed()->horizontalAngle();
}

void Viewshed360::setHorizontalAngle(double horizontalAngle)
{
  if (m_is360Mode)
    return;

  if (viewshed()->horizontalAngle() == horizontalAngle)
    return;

  viewshed()->setHorizontalAngle(horizontalAngle);

  emit horizontalAngleChanged();
}

double Viewshed360::verticalAngle() const
{
  if (m_is360Mode)
  {
    constexpr double verticalAngle360Mode = 90.0;
    return verticalAngle360Mode; // 360 mode
  }

  return viewshed()->verticalAngle();
}

void Viewshed360::setVerticalAngle(double verticalAngle)
{
  if (m_is360Mode)
    return;

  if (viewshed()->verticalAngle() == verticalAngle)
    return;

  viewshed()->setVerticalAngle(verticalAngle);

  emit verticalAngleChanged();
}

double Viewshed360::offsetZ() const
{
  constexpr double offsetZDefault = 0.0;
  return offsetZDefault;
}

void Viewshed360::setOffsetZ(double)
{
}

bool Viewshed360::isHeadingEnabled() const
{
  return !m_is360Mode;
}

bool Viewshed360::isPitchEnabled() const
{
  return !m_is360Mode;
}

bool Viewshed360::isOffsetZEnabled() const
{
  return false;
}

bool Viewshed360::is360Mode() const
{
  return m_is360Mode;
}

void Viewshed360::set360Mode(bool is360Mode)
{
  if (m_is360Mode == is360Mode)
    return;

  m_is360Mode = is360Mode;

  update360Mode(m_is360Mode);

  emit is360ModeChanged();
}

Viewshed* Viewshed360::viewshed() const
{
  return m_viewshed.data();
}

AnalysisOverlay* Viewshed360::analysisOverlay() const
{
  return m_analysisOverlay.data();
}
