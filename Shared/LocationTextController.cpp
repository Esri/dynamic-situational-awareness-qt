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

#include "LocationTextController.h"

#include "CoordinateFormatter.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "Scene.h"
#include "Surface.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

LocationTextController::LocationTextController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged,
          this, &LocationTextController::onGeoViewChanged);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged,
          this, &LocationTextController::onLocationChanged);

  m_coordinateFormatOptions << QStringLiteral("Degrees Minutes Seconds")
                            << QStringLiteral("Decimal Degrees")
                            << QStringLiteral("Decimal Minutes")
                            << QStringLiteral("UTM")
                            << QStringLiteral("MGRS")
                            << QStringLiteral("USNG")
                            << QStringLiteral("GeoRef")
                            << QStringLiteral("Gars");
}

LocationTextController::~LocationTextController()
{
}

QString LocationTextController::toolName() const
{
  return QStringLiteral("location text");
}

QString LocationTextController::currentLocationText() const
{
  return m_currentLocationText;
}

QString LocationTextController::currentElevationText() const
{
  return m_currentElevationText;
}

void LocationTextController::onLocationChanged(const Point& pt)
{
  // update location text
  m_currentLocationText = CoordinateFormatter::toLatitudeLongitude(pt, LatitudeLongitudeFormat::DegreesMinutesSeconds, 5);
  emit currentLocationTextChanged();

  // get elevation text
  if (!m_surface)
    return;

  m_surface->locationToElevation(pt);
}

void LocationTextController::onGeoViewChanged()
{
  Scene* scene = dynamic_cast<Scene*>(Toolkit::ToolResourceProvider::instance()->scene());
  if (scene)
  {
    m_surface = scene->baseSurface();

    connect(m_surface, &Surface::locationToElevationCompleted, this, [this](QUuid, double elevation)
    {
      m_currentElevationText = QString::number(elevation);
      emit currentElevationTextChanged();
    });
  }
}

QStringList LocationTextController::coordinateFormatOptions() const
{
  return m_coordinateFormatOptions;
}
