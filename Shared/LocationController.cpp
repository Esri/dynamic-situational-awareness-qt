
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

#include "LocationController.h"

// dsa app headers
#include "GPXLocationSimulator.h"
#include "LocationDisplay3d.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "GraphicsOverlay.h"
#include "ModelSceneSymbol.h"
#include "PictureMarkerSymbol.h"
#include "Point.h"
#include "SceneQuickView.h"
#include "SimpleRenderer.h"

// Qt headers
#include <QCompass>
#include <QDir>
#include <QFile>

// STL headers
#include <cmath>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString LocationController::SIMULATE_LOCATION_PROPERTYNAME = "SimulateLocation";
const QString LocationController::GPX_FILE_PROPERTYNAME = "GpxFile";
const QString LocationController::RESOURCE_DIRECTORY_PROPERTYNAME = "ResourceDirectory";

/*!
  \class Dsa::LocationController
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Tool controller for handling the current location.
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
LocationController::LocationController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_locationDisplay3d(new LocationDisplay3d(this))
{
  connect(this, &LocationController::locationChanged, Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::onLocationChanged);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &LocationController::updateGeoView);

  updateGeoView();

  Toolkit::ToolManager::instance().addTool(this);
}

/*!
  \brief Destructor.
 */
LocationController::~LocationController()
{
}

/*!
  \internal
 */
void LocationController::initPositionInfoSource()
{
  if (isSimulationEnabled() && dynamic_cast<GPXLocationSimulator*>(m_positionSource))
    return;

  clearPositionInfoSource();

  if (isSimulationEnabled())
  {
    auto gpxLocationSimulator = new GPXLocationSimulator(this);

    if (!m_gpxFilePath.isEmpty())
      gpxLocationSimulator->setGpxFile(m_gpxFilePath);

    m_positionSource = gpxLocationSimulator;

    connect(gpxLocationSimulator, &GPXLocationSimulator::headingChanged, this,
    [this](double heading)
    {
      if (m_lastKnownHeading == heading)
        return;

      m_lastKnownHeading = heading;

      emit headingChanged(heading);
    });
  }
  else
  {
    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);

    m_compass = new QCompass(this);

    connect(m_compass, &QCompass::readingChanged, this, [this]()
    {
      if (!m_compass)
        return;

      QCompassReading* reading = m_compass->reading();
      if (!reading)
        return;

      emit headingChanged(static_cast<double>(reading->azimuth()));
    });
  }

  connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated, this,
  [this](const QGeoPositionInfo& update)
  {
    if (!update.isValid())
      return;

    const auto pos = update.coordinate();

    if (!pos.isValid())
      return;

    switch (pos.type())
    {
      case QGeoCoordinate::Coordinate2D:
        m_currentLocation = Point(pos.longitude(), pos.latitude(), SpatialReference::wgs84());
        break;
      case QGeoCoordinate::Coordinate3D:
        m_currentLocation = Point(pos.longitude(), pos.latitude(), pos.altitude(), SpatialReference::wgs84());
        break;
      case QGeoCoordinate::InvalidCoordinate:
      default:
        return;
    }

    emit locationChanged(m_currentLocation);
  });

  // apply position source and compass to the location display
  m_locationDisplay3d->setPositionSource(m_positionSource);
  m_locationDisplay3d->setCompass(m_compass);
}

/*!
  \internal
 */
void LocationController::clearPositionInfoSource()
{
  if (m_positionSource)
  {
    m_positionSource->stopUpdates();
    delete m_positionSource;
    m_positionSource = nullptr;
  }

  if (m_compass)
  {
    m_compass->stop();
    delete m_compass;
    m_compass = nullptr;
  }
}

/*!
  \brief Returns the name of the tool - \c "location".
 */
QString LocationController::toolName() const
{
  return QStringLiteral("location");
}

/*! \brief Sets any values in \a properties which are relevant for the location tool.
 *
 * This tool will use the following key/value pairs in the \a properties map if they are set:
 *
 * \list
 *  \li \c SimulateLocation - Whether the app's location should be simulated.
 *  \li \c GpxFile - The path of the GPX file for simulated positions.
 *  \li \c ResourceDirectory - The directory containing icons for the location display.
 * \endlist
 */
void LocationController::setProperties(const QVariantMap& properties)
{
  const bool simulate = QString::compare(properties[SIMULATE_LOCATION_PROPERTYNAME].toString(), QString("true"), Qt::CaseInsensitive) == 0;
  setGpxFilePath(properties[GPX_FILE_PROPERTYNAME].toString());
  setSimulationEnabled(simulate);
  setIconDataPath(properties[RESOURCE_DIRECTORY_PROPERTYNAME].toString());
}

/*!
  \property LocationController::enabled
  \brief Returns whether the tool is enabled.
 */
bool LocationController::isEnabled() const
{
  return m_enabled;
}

/*!
  \brief Sets the tool to be \a enabled.
 */
void LocationController::setEnabled(bool enabled)
{
  if (m_enabled == enabled)
    return;

  initPositionInfoSource();

  if (enabled)
  {
    m_positionSource->startUpdates();
    if (m_compass)
      m_compass->start();
  }
  else
  {
    m_positionSource->stopUpdates();
    if (m_compass)
      m_compass->stop();
  }

  m_enabled = enabled;
  emit enabledChanged();
}

/*!
  \property LocationController::locationVisible
  \brief Returns whether the location is visible.
 */
bool LocationController::isLocationVisible() const
{
  return m_locationDisplay3d->isStarted();
}

/*!
  \brief Sets whether the location is visible to \a visible.
 */
void LocationController::setLocationVisible(bool visible)
{
  if (m_locationDisplay3d->isStarted() == visible)
    return;

  if (visible)
    m_locationDisplay3d->start();
  else
    m_locationDisplay3d->stop();

  emit locationVisibleChanged();
}

/*!
  \property LocationController::simulationEnabled
  \brief Returns whether the location is simulated.
 */
bool LocationController::isSimulationEnabled() const
{
  return m_simulated;
}

/*!
  \brief Sets whether the location is simulated to \a simulated.
 */
void LocationController::setSimulationEnabled(bool simulated)
{
  if (m_simulated == simulated)
    return;

  m_simulated = simulated;

  initPositionInfoSource();

  if (isEnabled())
  {
    m_positionSource->startUpdates();
    if (m_compass)
      m_compass->start();
  }

  emit simulationEnabledChanged();
  emit propertyChanged(SIMULATE_LOCATION_PROPERTYNAME, m_simulated);
}

/*!
  \brief Returns the current location.
 */
Point LocationController::currentLocation() const
{
  return m_currentLocation;
}

/*!
  \brief Returns the location display for 3D apps.
 */
LocationDisplay3d* LocationController::locationDisplay() const
{
  return m_locationDisplay3d;
}

/*!
  \property LocationController::gpxFilePath
  \brief Returns the file path of the GPX file.
 */
QString LocationController::gpxFilePath() const
{
  return m_gpxFilePath;
}

/*!
  \brief Sets the file path of the GPX file to \a gpxFilePath.
 */
void LocationController::setGpxFilePath(const QString& gpxFilePath)
{
  if (m_gpxFilePath == gpxFilePath)
    return;

  if (!QFile::exists(gpxFilePath))
  {
    emit toolErrorOccurred(QStringLiteral("GPX File missing"), QString("Could not find %1").arg(gpxFilePath));
    return;
  }

  if (isSimulationEnabled())
  {
    initPositionInfoSource();
    static_cast<GPXLocationSimulator*>(m_positionSource)->setGpxFile(gpxFilePath);

    if (isEnabled())
    {
      m_positionSource->startUpdates();
      if (m_compass)
        m_compass->start();
    }
  }

  m_gpxFilePath = gpxFilePath;
  emit gpxFilePathChanged();
  emit propertyChanged(GPX_FILE_PROPERTYNAME, m_gpxFilePath);
}

/*!
  \internal
 */
void LocationController::updateGeoView()
{
  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (geoView)
  {
    geoView->graphicsOverlays()->append(m_locationDisplay3d->locationOverlay());

    constexpr float symbolSize = 25.0;

    auto pictureMarkerSymbol = new PictureMarkerSymbol(iconSymbol(), this);
    pictureMarkerSymbol->setWidth(symbolSize);
    pictureMarkerSymbol->setHeight(symbolSize);
    pictureMarkerSymbol->setRotationType(RotationType::Geographic);
    pictureMarkerSymbol->setAngleAlignment(SymbolAngleAlignment::Map);
    m_locationDisplay3d->setDefaultSymbol(pictureMarkerSymbol);
  }
}

/*!
  \brief Sets the icon data path to \a dataPath.
 */
void LocationController::setIconDataPath(const QString& dataPath)
{
  if (dataPath == m_iconDataPath)
    return;

  m_iconDataPath = dataPath;
  emit propertyChanged(RESOURCE_DIRECTORY_PROPERTYNAME, m_iconDataPath);
}

/*!
  \brief Returns the QImage of the model symbol used for location display.
 */
QImage LocationController::iconImage() const
{
  const QString imagePath = m_iconDataPath + "/navigation.png";

  return (QFile::exists(imagePath)) ? QImage(imagePath)
                                    : QImage(":Resources/icons/xhdpi/navigation.png");
}

} // Dsa

// Signal Documentation
/*!
  \fn void LocationController::locationChanged(const Esri::ArcGISRuntime::Point& newLocation);
  \brief Signal emitted when the location changes to \a newLocation.
 */

/*!
  \fn void LocationController::headingChanged(double newHeading);
  \brief Signal emitted when the heading changes to \a newHeading.
 */

/*!
  \fn void LocationController::gpxFilePathChanged();
  \brief Signal emitted when GPX file path changes.
 */

/*!
  \fn void LocationController::enabledChanged();
  \brief Signal emitted when the enabled property changes.
 */

/*!
  \fn void LocationController::locationVisibleChanged();
  \brief Signal emitted when the locationVisible property changes.
 */

/*!
  \fn void LocationController::simulationEnabledChanged();
  \brief Signal emitted when simulationEnabled property changes.
 */

/*!
  \fn void LocationController::toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  \brief Signal emitted when an error occurs.

  An \a errorMessage and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */
