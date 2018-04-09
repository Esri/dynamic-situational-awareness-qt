// Copyright 2017 ESRI
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

#include "LocationDisplay3d.h"

// example app headers
#include "GPXLocationSimulator.h"

// C++ API headers
#include "GraphicsOverlay.h"
#include "SimpleRenderer.h"

// Qt headers
#include <QCompass>

// STL headers
#include <cmath>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

static const QString s_headingAttribute{"heading"};

LocationDisplay3d::LocationDisplay3d(QObject* parent) :
  QObject(parent),
  m_locationOverlay(new GraphicsOverlay(this)),
  m_locationGraphic(new Graphic(this))
{
  m_locationOverlay->setOverlayId(QStringLiteral("SCENEVIEWLOCATIONOVERLAY"));
  m_locationOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
  m_locationOverlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
  m_locationOverlay->setVisible(false);

  m_locationGraphic->attributes()->insertAttribute(s_headingAttribute, 0.0);
  m_locationOverlay->graphics()->append(m_locationGraphic);
}

LocationDisplay3d::~LocationDisplay3d()
{
}

void LocationDisplay3d::start()
{
  if (m_geoPositionInfoSource)
    m_geoPositionInfoSource->startUpdates();

  m_locationOverlay->setVisible(true);

  m_isStarted = true;
}

void LocationDisplay3d::stop()
{
  m_locationOverlay->setVisible(false);
  m_lastKnownLocation = Point();

  m_isStarted = false;
}

bool LocationDisplay3d::isStarted() const
{
  return m_isStarted;
}

QGeoPositionInfoSource* LocationDisplay3d::positionSource() const
{
  return m_geoPositionInfoSource;
}

void LocationDisplay3d::setPositionSource(QGeoPositionInfoSource* positionSource)
{
  m_geoPositionInfoSource = positionSource;

  if (!m_geoPositionInfoSource)
    return;

  if (m_positionErrorConnection)
    disconnect(m_positionErrorConnection);

  if (m_positionUpdateConnection)
    disconnect(m_positionUpdateConnection);

  m_positionErrorConnection = connect(m_geoPositionInfoSource, static_cast<void (QGeoPositionInfoSource::*)(QGeoPositionInfoSource::Error)>(&QGeoPositionInfoSource::error), this,
                                      [this](QGeoPositionInfoSource::Error error)
  {
    if (error != QGeoPositionInfoSource::Error::NoError)
    {
      postLastKnownLocationUpdate();
    }
  });

  m_positionUpdateConnection = connect(m_geoPositionInfoSource, &QGeoPositionInfoSource::positionUpdated, this,
                                       [this](const QGeoPositionInfo& update)
  {
    const auto pos = update.coordinate();

    if (!pos.isValid())
    {
      postLastKnownLocationUpdate();
      return;
    }

    // display position 10m off the ground
    constexpr double elevatedZ = 10.0;
    m_lastKnownLocation = Point(pos.longitude(), pos.latitude(), elevatedZ, SpatialReference::wgs84());
    m_locationGraphic->setGeometry(m_lastKnownLocation);

    emit locationChanged(m_lastKnownLocation);
  });

  auto* gpxLocationSimulator = dynamic_cast<GPXLocationSimulator*>(m_geoPositionInfoSource);
  if (gpxLocationSimulator)
  {
    if (m_headingConnection)
      disconnect(m_headingConnection);

    m_headingConnection = connect(gpxLocationSimulator, &GPXLocationSimulator::headingChanged, this, [this](double heading)
    {
      m_locationGraphic->attributes()->replaceAttribute(s_headingAttribute, heading);
    });
  }

  if (isStarted())
    m_geoPositionInfoSource->startUpdates();
}

QCompass* LocationDisplay3d::compass() const
{
  return m_compass;
}

void LocationDisplay3d::setCompass(QCompass* compass)
{
  m_compass = compass;

  if (!m_compass)
    return;

  if (m_headingConnection)
    disconnect(m_headingConnection);

  m_headingConnection = connect(m_compass, &QCompass::readingChanged, this, [this]()
  {
    if (!m_compass)
      return;

    QCompassReading* reading = m_compass->reading();
    if (!reading)
      return;

    m_locationGraphic->attributes()->replaceAttribute(s_headingAttribute, static_cast<double>(reading->azimuth()));

    emit headingChanged();
  });

  m_compass->start();
}

GraphicsOverlay* LocationDisplay3d::locationOverlay() const
{
  return m_locationOverlay;
}

Graphic* LocationDisplay3d::locationGraphic() const
{
  return m_locationGraphic;
}

Symbol* LocationDisplay3d::defaultSymbol() const
{
  return m_defaultSymbol;
}

void LocationDisplay3d::setDefaultSymbol(Symbol* defaultSymbol)
{
  m_defaultSymbol = defaultSymbol;

  if (!m_locationRenderer)
  {
    m_locationRenderer = new SimpleRenderer(defaultSymbol, this);
    RendererSceneProperties renderProperties = m_locationRenderer->sceneProperties();
    renderProperties.setHeadingExpression(QString("[%1]").arg(s_headingAttribute));
    m_locationRenderer->setSceneProperties(renderProperties);

    m_locationOverlay->setRenderer(m_locationRenderer);
  }
  else
  {
    m_locationRenderer->setSymbol(defaultSymbol);
  }
}

void LocationDisplay3d::postLastKnownLocationUpdate()
{
  if (m_lastKnownLocation.isEmpty())
    return;

  m_locationGraphic->setGeometry(m_lastKnownLocation);

  emit locationChanged(m_lastKnownLocation);
}

} // Dsa
