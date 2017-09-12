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

#include "LocationController.h"

#include <QDir>
#include <QCompass>
#include "GPXLocationSimulator.h"
#include <QtDebug>
#include "DsaUtility.h"
#include "SceneQuickView.h"
#include "Point.h"
#include <QGeoPositionInfoSource>

using namespace Esri::ArcGISRuntime;

LocationController::LocationController(QObject* parent) :
  QObject(parent)
{
  // placeholder until we have ToolManager
  DsaUtility::tools.append(this);
}

LocationController::~LocationController()
{
}

void LocationController::initPositionInfoSource(bool simulated)
{
  if (simulated && !m_simulator)
  {
    m_simulator = new GPXLocationSimulator(this);

    connect(m_simulator, &GPXLocationSimulator::positionUpdateAvailable, this,
    [this](const Point& pos, double heading)
    {
      m_lastKnownHeading = heading;

      emit positionChanged(pos);
      emit headingChanged(heading);
      emit relativeHeadingChanged(heading - m_lastViewHeading);
    });
  }
  else if (!simulated && !m_positionSource)
  {
    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);

    connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated, this,
    [this](const QGeoPositionInfo& update)
    {
      if (!update.isValid())
        return;

      const auto pos = update.coordinate();

      if (!pos.isValid())
        return;

      Point newPosition;
      switch (pos.type())
      {
        case QGeoCoordinate::Coordinate2D:
          newPosition = Point(pos.longitude(), pos.latitude(), SpatialReference::wgs84());
          break;
        case QGeoCoordinate::Coordinate3D:
          newPosition = Point(pos.longitude(), pos.latitude(), pos.altitude(), SpatialReference::wgs84());
          break;
        case QGeoCoordinate::InvalidCoordinate:
        default:
          return;
      }

      emit positionChanged(newPosition);
    });

    m_compass = new QCompass(this);

    connect(m_compass, &QCompass::readingChanged, this, [this]()
    {
      QCompassReading* reading = m_compass->reading();
      if (!reading)
        return;

      emit headingChanged(static_cast<double>(reading->azimuth()));
    });
  }
}

QString LocationController::toolName() const
{
  return QStringLiteral("location");
}

bool LocationController::enabled() const
{
  return m_enabled;
}

void LocationController::setEnabled(bool enabled)
{
  if (m_enabled == enabled)
    return;

  initPositionInfoSource(m_simulated);

  if (m_simulated)
  {
    if (enabled)
      m_simulator->startUpdates();
    else
      m_simulator->stopUpdates();
  }
  else
  {
    if (enabled)
    {
      m_positionSource->startUpdates();
      m_compass->start();
    }
    else
    {
      m_positionSource->stopUpdates();
      m_compass->stop();
    }
  }

  m_enabled = enabled;
  emit enabledChanged();
}

bool LocationController::simulated() const
{
  return m_simulated;
}

void LocationController::setSimulated(bool simulated)
{
  if (m_simulated == simulated)
    return;

  m_simulated = simulated;
  emit simulatedChanged();
}

QUrl LocationController::gpxFilePath() const
{
  return m_gpxFilePath;
}

void LocationController::setGpxFilePath(const QUrl& gpxFilePath)
{
  if (m_gpxFilePath == gpxFilePath)
    return;

  initPositionInfoSource(true); // ignore m_simulated, we need to init the simulator now

  m_simulator->setGpxFile(gpxFilePath.toLocalFile());

  m_gpxFilePath = gpxFilePath;
  emit gpxFilePathChanged();
}

QUrl LocationController::defaultFileSearchPath() const
{
  return QUrl::fromLocalFile(DsaUtility::dataPath());
}

void LocationController::setRelativeHeadingSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView)
{
  connect(sceneView, &SceneQuickView::viewpointChanged, this,
  [sceneView, this]()
  {
    const auto sceneViewHeading = sceneView->currentViewpointCamera().heading();
    if (std::abs(m_lastViewHeading - sceneViewHeading) < 0.1)
      return;

    m_lastViewHeading = sceneViewHeading;

    // keep the orientation correct if we're not doing any updates
    if (!m_enabled)
      emit relativeHeadingChanged(m_lastKnownHeading + m_lastViewHeading);
  });
}
