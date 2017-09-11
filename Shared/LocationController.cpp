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
#include <QGeoPositionInfoSource>
#include <QCompass>
#include "GPSSimulator.h"
#include <QtDebug>
#include "DsaUtility.h"

#include "Point.h"

using namespace Esri::ArcGISRuntime;

LocationController::LocationController(QObject* parent) :
  QObject(parent),
  m_simulator(new GPSSimulator(this))
{
  // placeholder until we have ToolManager
  DsaUtility::tools.append(this);

  connect(m_simulator, &GPSSimulator::positionUpdateAvailable, this,
  [this](const Point& pos, double heading)
  {
    emit positionChanged(pos);
    emit headingChanged(heading);
  });
}

LocationController::~LocationController()
{
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

  if (m_simulated)
  {
    if (enabled)
    {
      if (!m_simulator->isStarted())
        m_simulator->startSimulation();
      else
        m_simulator->resumeSimulation();
    }
    else
    {
      m_simulator->pauseSimulation();
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

  m_simulator->setGpxFile(gpxFilePath.toLocalFile());

  m_gpxFilePath = gpxFilePath;
  emit gpxFilePathChanged();
}

QUrl LocationController::defaultFileSearchPath() const
{
  return QUrl::fromLocalFile(DsaUtility::dataPath());
}
