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

#include "LocationDisplay3d.h"

// dsa app headers
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

/*!
  \class Dsa::LocationDisplay3d
  \inmodule Dsa
  \inherits QObject
  \brief Class for managing the display of the current location in 3D.

  The position is displayed as a \l Esri::ArcGISRuntime::Graphic in
  a \l Esri::ArcGISRuntime::GraphicsOverlay.
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
LocationDisplay3d::LocationDisplay3d(QObject* parent) :
  QObject(parent),
  m_locationOverlay(new GraphicsOverlay(this)),
  m_locationGraphic(new Graphic(this))
{
  m_locationOverlay->setOverlayId(QStringLiteral("SCENEVIEWLOCATIONOVERLAY"));
  m_locationOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::DrapedFlat));
  m_locationOverlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
  m_locationOverlay->setVisible(false);

  m_locationGraphic->attributes()->insertAttribute(s_headingAttribute, 0.0);
  m_locationOverlay->graphics()->append(m_locationGraphic);
}

/*!
  \brief Destructor.
 */
LocationDisplay3d::~LocationDisplay3d()
{
}

/*!
  \brief Starts the location display.
 */
void LocationDisplay3d::start()
{
  if (m_geoPositionInfoSource)
    m_geoPositionInfoSource->startUpdates();

  m_locationOverlay->setVisible(true);

  m_isStarted = true;
}

/*!
  \brief Stops the location display.
 */
void LocationDisplay3d::stop()
{
  m_locationOverlay->setVisible(false);
  m_lastKnownLocation = Point();

  m_isStarted = false;
}

/*!
  \brief Returns whether the location display is started.
 */
bool LocationDisplay3d::isStarted() const
{
  return m_isStarted;
}

/*!
  \brief Returns the position source for the location display.
 */
QGeoPositionInfoSource* LocationDisplay3d::positionSource() const
{
  return m_geoPositionInfoSource;
}

/*!
  \brief Sets the position source for the location display to \a positionSource.
 */
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

/*!
  \brief Returns the compass for the location display.
 */
QCompass* LocationDisplay3d::compass() const
{
  return m_compass;
}

/*!
  \brief Sets the compass for the location display to \a compass.
 */
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

/*!
  \brief Returns the overlay for the location display.
 */
GraphicsOverlay* LocationDisplay3d::locationOverlay() const
{
  return m_locationOverlay;
}

/*!
  \brief Returns the graphic for the location display.
 */
Graphic* LocationDisplay3d::locationGraphic() const
{
  return m_locationGraphic;
}

/*!
  \brief Returns the default symbol for the location display.
 */
Symbol* LocationDisplay3d::defaultSymbol() const
{
  return m_defaultSymbol;
}

/*!
  \brief Sets the default symbol for the location display to \a defaultSymbol.
 */
void LocationDisplay3d::setDefaultSymbol(Symbol* defaultSymbol)
{
  m_defaultSymbol = defaultSymbol;

  if (!m_locationRenderer)
  {
    m_locationRenderer = new SimpleRenderer(defaultSymbol, this);

    m_locationRenderer->setRotationExpression(QString("[%1]").arg(s_headingAttribute));

    m_locationOverlay->setRenderer(m_locationRenderer);
  }
  else
  {
    m_locationRenderer->setSymbol(defaultSymbol);
  }
}

/*!
  \internal
 */
void LocationDisplay3d::postLastKnownLocationUpdate()
{
  if (m_lastKnownLocation.isEmpty())
    return;

  m_locationGraphic->setGeometry(m_lastKnownLocation);

  emit locationChanged(m_lastKnownLocation);
}

} // Dsa

// Signal Documentation
/*!
  \fn void LocationDisplay3d::locationChanged(const Esri::ArcGISRuntime::Point& location);
  \brief Signal emitted when the \a location changes.
 */

/*!
  \fn void LocationDisplay3d::headingChanged();
  \brief Signal emitted when the heading changes.
 */

