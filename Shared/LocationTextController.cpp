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

const QString LocationTextController::COORDINATE_FORMAT_PROPERTYNAME = "CoordinateFormat";

LocationTextController::LocationTextController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged,
          this, &LocationTextController::onGeoViewChanged);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged,
          this, &LocationTextController::onLocationChanged);

  m_coordinateFormatOptions << QStringLiteral("DMS")
                            << QStringLiteral("DD")
                            << QStringLiteral("DDM")
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
  m_currentLocationText = formatCoordinate(pt);
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

void LocationTextController::setProperties(const QVariantMap& properties)
{
  const QString defaultFormat = properties[COORDINATE_FORMAT_PROPERTYNAME].toString();
  setCoordinateFormat(defaultFormat);
}

void LocationTextController::setCoordinateFormat(const QString& format)
{
  m_coordinateFormat = format;

  if (coordinateFormat() == "DD")
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DecimalDegrees, 5);
    };
  }
  else if (coordinateFormat() == "DDM")
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DegreesDecimalMinutes, 5);
    };
  }
  else if (coordinateFormat() == "UTM")
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUtm(p, UtmConversionMode::NorthSouthIndicators, true);
    };
  }
  else if (coordinateFormat() == "MGRS")
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toMgrs(p, MgrsConversionMode::Automatic, 5, true);
    };
  }
  else if (coordinateFormat() == "USNG")
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUsng(p, 5, true);
    };
  }
  else if (coordinateFormat() == "GeoRef")
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toGeoRef(p, 5);
    };
  }
  else if (coordinateFormat() == "Gars")
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toGars(p);
    };
  }
  else // DMS
  {
    formatCoordinate =  [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DegreesMinutesSeconds, 3);
    };
  }
}

QString LocationTextController::coordinateFormat() const
{
  return m_coordinateFormat;
}
