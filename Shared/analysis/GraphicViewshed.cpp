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

#include "GraphicViewshed.h"
#include "Graphic.h"
#include "GeoElementViewshed.h"
#include "AnalysisOverlay.h"

#include <cmath>

using namespace Esri::ArcGISRuntime;

GraphicViewshed::GraphicViewshed(Graphic* locationGraphic, AnalysisOverlay* analysisOverlay,
                                                 const QString& headingAttribute, const QString& pitchAttribute, QObject* parent) :
  AbstractViewshed(new GeoElementViewshed(locationGraphic, 120.0, 90.0, 30.0, 1000.0, 0.0, 0.0, parent), analysisOverlay, parent),
  m_locationGraphic(locationGraphic),
  m_headingAttribute(headingAttribute),
  m_pitchAttribute(pitchAttribute)
{
}

GraphicViewshed::~GraphicViewshed()
{
}

double GraphicViewshed::heading() const
{
  if (m_headingAttribute.isEmpty())
    return static_cast<GeoElementViewshed*>(viewshed())->headingOffset();

  return m_locationGraphic->attributes()->attributeValue(m_headingAttribute).toDouble();
}

void GraphicViewshed::setHeading(double heading)
{
  if (m_headingAttribute.isEmpty())
  {
    if (static_cast<GeoElementViewshed*>(viewshed())->headingOffset() == heading)
      return;

    static_cast<GeoElementViewshed*>(viewshed())->setHeadingOffset(heading);

    if (!m_viewsheds360Offsets.isEmpty())
    {
      double headingOffset1 = heading + 120.0;
      if (headingOffset1 > 360)
        headingOffset1 -= 360;

      static_cast<GeoElementViewshed*>(m_viewsheds360Offsets[0])->setHeadingOffset(headingOffset1);
      static_cast<GeoElementViewshed*>(m_viewsheds360Offsets[1])->setHeadingOffset(headingOffset1 + 120.0);
    }
  }
  else
  {
    auto attributes = m_locationGraphic->attributes();
    if (attributes->attributeValue(m_headingAttribute).toDouble() == heading)
      return;

    attributes->replaceAttribute(m_headingAttribute, heading);
  }

  emit headingChanged();
}

double GraphicViewshed::pitch() const
{
  if (m_pitchAttribute.isEmpty())
    return static_cast<GeoElementViewshed*>(viewshed())->pitchOffset();

  return m_locationGraphic->attributes()->attributeValue(m_pitchAttribute).toDouble();
}

void GraphicViewshed::setPitch(double pitch)
{
  if (m_pitchAttribute.isEmpty())
  {
    if (static_cast<GeoElementViewshed*>(viewshed())->pitchOffset() == pitch)
      return;

    static_cast<GeoElementViewshed*>(viewshed())->setPitchOffset(pitch);
  }
  else
  {
    auto attributes = m_locationGraphic->attributes();
    if (attributes->attributeValue(m_pitchAttribute).toDouble() == pitch)
      return;

    attributes->replaceAttribute(m_pitchAttribute, pitch);
  }

  emit pitchChanged();
}

bool GraphicViewshed::isHeadingEnabled() const
{
  return !is360Mode() && m_headingAttribute.isEmpty();
}

bool GraphicViewshed::isPitchEnabled() const
{
  return !is360Mode() && m_pitchAttribute.isEmpty();
}

double GraphicViewshed::offsetZ() const
{
  return static_cast<GeoElementViewshed*>(viewshed())->offsetZ();
}

void GraphicViewshed::setOffsetZ(double offsetZ)
{
  static_cast<GeoElementViewshed*>(viewshed())->setOffsetZ(offsetZ);
}

QString GraphicViewshed::headingAttribute() const
{
  return m_headingAttribute;
}

QString GraphicViewshed::pitchAttribute() const
{
  return m_pitchAttribute;
}

AbstractViewshed::AnalysisType GraphicViewshed::analysisType() const
{
  return AbstractViewshed::AnalysisType::GraphicViewshed;
}

void GraphicViewshed::update360Mode(bool is360Mode)
{
  if (is360Mode && m_viewsheds360Offsets.isEmpty())
  {
    double headingOffset1 = static_cast<GeoElementViewshed*>(viewshed())->headingOffset() + 120.0;
    if (headingOffset1 > 360)
      headingOffset1 -= 360;

    auto viewshedOffset1 = new GeoElementViewshed(m_locationGraphic, 120.0, verticalAngle(),
                                                  minDistance(), maxDistance(), headingOffset1, 0.0, this);

    viewshedOffset1->setOffsetZ(5.0);
    viewshedOffset1->setVisible(isVisible());
    m_analysisOverlay->analyses()->append(viewshedOffset1);
    m_viewsheds360Offsets.append(viewshedOffset1);

    auto viewshedOffset2 = new GeoElementViewshed(m_locationGraphic, 120.0, verticalAngle(),
                                                  minDistance(), maxDistance(), headingOffset1 + 120.0, 0.0, this);

    viewshedOffset2->setOffsetZ(5.0);
    viewshedOffset2->setVisible(isVisible());
    m_analysisOverlay->analyses()->append(viewshedOffset2);
    m_viewsheds360Offsets.append(viewshedOffset2);
  }

  viewshed()->setHorizontalAngle(120.0);
  viewshed()->setVerticalAngle(90.0);

  emit horizontalAngleChanged();
  emit verticalAngleChanged();

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setVisible(isVisible() && is360Mode);
  }

  emit headingEnabledChanged();
  emit pitchEnabledChanged();
}
