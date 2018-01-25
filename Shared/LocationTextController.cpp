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

const QString LocationTextController::COORDINATE_FORMAT_PROPERTYNAME = QStringLiteral("CoordinateFormat");
const QString LocationTextController::USE_GPS_PROPERTYNAME = QStringLiteral("UseGpsForElevation");
const QString LocationTextController::UNIT_OF_MEASUREMENT_PROPERTYNAME = QStringLiteral("UnitOfMeasurement");
const QString LocationTextController::DMS = QStringLiteral("DMS");
const QString LocationTextController::DD = QStringLiteral("DD");
const QString LocationTextController::DDM = QStringLiteral("DDM");
const QString LocationTextController::UTM = QStringLiteral("UTM");
const QString LocationTextController::MGRS = QStringLiteral("MGRS");
const QString LocationTextController::USNG = QStringLiteral("USNG");
const QString LocationTextController::GeoRef = QStringLiteral("GeoRef");
const QString LocationTextController::Gars = QStringLiteral("Gars");
const QString LocationTextController::Meters = QStringLiteral("meters");
const QString LocationTextController::Feet = QStringLiteral("feet");

LocationTextController::LocationTextController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged,
          this, &LocationTextController::onGeoViewChanged);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged,
          this, &LocationTextController::onLocationChanged);

  m_coordinateFormatOptions << DMS << DD << DDM << UTM << MGRS << USNG << GeoRef << Gars;
  m_units << Meters << Feet;
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
  m_currentLocationText = QString("%1 (%2)").arg(formatCoordinate(pt), m_coordinateFormat);
  emit currentLocationTextChanged();

  // get elevation text
  if (!m_surface)
    return;

  if (m_useGpsForElevation)
    formatElevationText(pt.z());
  else
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
      formatElevationText(elevation);
    });
  }
}

QStringList LocationTextController::coordinateFormatOptions() const
{
  return m_coordinateFormatOptions;
}

void LocationTextController::setProperties(const QVariantMap& properties)
{
  setCoordinateFormat(properties[COORDINATE_FORMAT_PROPERTYNAME].toString());
  setUseGpsForElevation(properties[USE_GPS_PROPERTYNAME].toBool());
  setUnitOfMeasurement(properties[UNIT_OF_MEASUREMENT_PROPERTYNAME].toString());
}

void LocationTextController::setCoordinateFormat(const QString& format)
{
  m_coordinateFormat = format;
  emit propertyChanged(COORDINATE_FORMAT_PROPERTYNAME, format);
  emit coordinateFormatChanged();

  if (coordinateFormat() == DD)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DecimalDegrees, 5);
    };
  }
  else if (coordinateFormat() == DDM)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DegreesDecimalMinutes, 5);
    };
  }
  else if (coordinateFormat() == UTM)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUtm(p, UtmConversionMode::NorthSouthIndicators, true);
    };
  }
  else if (coordinateFormat() == MGRS)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toMgrs(p, MgrsConversionMode::Automatic, 5, true);
    };
  }
  else if (coordinateFormat() == USNG)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUsng(p, 5, true);
    };
  }
  else if (coordinateFormat() == GeoRef)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toGeoRef(p, 5);
    };
  }
  else if (coordinateFormat() == Gars)
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

bool LocationTextController::useGpsForElevation() const
{
  return m_useGpsForElevation;
}

void LocationTextController::setUseGpsForElevation(bool useGps)
{
  m_useGpsForElevation = useGps;
  emit propertyChanged(USE_GPS_PROPERTYNAME, useGps);
  emit useGpsForElevationChanged();
}

void LocationTextController::formatElevationText(double elevation)
{
  if (unitOfMeasurement() == Feet)
  {
    elevation = elevation * 3.280839895;
  }
  m_currentElevationText = QString("%1 %2 MSL").arg(QString::number(elevation), unitOfMeasurement());
  emit currentElevationTextChanged();
}

QStringList LocationTextController::units() const
{
  return m_units;
}

QString LocationTextController::unitOfMeasurement() const
{
  return m_unitOfMeasurement;
}

void LocationTextController::setUnitOfMeasurement(const QString& unit)
{
  m_unitOfMeasurement = unit;
  emit propertyChanged(UNIT_OF_MEASUREMENT_PROPERTYNAME, unit);
  emit unitOfMeasurementChanged();
}
