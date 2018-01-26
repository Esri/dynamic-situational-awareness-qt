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

// constant strings used for properties in the config file
const QString LocationTextController::COORDINATE_FORMAT_PROPERTYNAME = QStringLiteral("CoordinateFormat");
const QString LocationTextController::USE_GPS_PROPERTYNAME = QStringLiteral("UseGpsForElevation");
const QString LocationTextController::UNIT_OF_MEASUREMENT_PROPERTYNAME = QStringLiteral("UnitOfMeasurement");
// constant strings used for display in the options
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

// Returns the Tool Name
QString LocationTextController::toolName() const
{
  return QStringLiteral("Location Text");
}

// Returns the location text string for display in QML
QString LocationTextController::currentLocationText() const
{
  return m_currentLocationText;
}

// Returns the elevation text string for display in QML
QString LocationTextController::currentElevationText() const
{
  return m_currentElevationText;
}

// Slot for Toolkit::ToolResourceProvider::locationChanged
void LocationTextController::onLocationChanged(const Point& pt)
{
  // update location text
  m_currentLocationText = QString("%1 (%2)").arg(formatCoordinate(pt), m_coordinateFormat);
  emit currentLocationTextChanged();

  // update the elevation text
  if (m_useGpsForElevation)
    formatElevationText(pt.z());
  else
  {
    if (!m_surface)
      return;

    m_surface->locationToElevation(pt);
  }
}

// Slot to obtain the Scene's base surface
void LocationTextController::onGeoViewChanged()
{
  Scene* scene = Toolkit::ToolResourceProvider::instance()->scene();
  if (scene)
  {
    m_surface = scene->baseSurface();

    // connect the Surface::locationToElevationCompleted signal
    connect(m_surface, &Surface::locationToElevationCompleted, this, [this](QUuid, double elevation)
    {
      // format the elevation for display in QML
      formatElevationText(elevation);
    });
  }
}

// Returns a string list of coordinate format options
QStringList LocationTextController::coordinateFormatOptions() const
{
  return m_coordinateFormatOptions;
}

// Set properties from the configuration file
void LocationTextController::setProperties(const QVariantMap& properties)
{
  setCoordinateFormat(properties[COORDINATE_FORMAT_PROPERTYNAME].toString());
  setUseGpsForElevation(properties[USE_GPS_PROPERTYNAME].toBool());
  setUnitOfMeasurement(properties[UNIT_OF_MEASUREMENT_PROPERTYNAME].toString());
}

// Changes the coordinate format
void LocationTextController::setCoordinateFormat(const QString& format)
{
  m_coordinateFormat = format;
  emit propertyChanged(COORDINATE_FORMAT_PROPERTYNAME, format);
  emit coordinateFormatChanged();

  // use std::function to change the lambda that the formatCoordinate member points to
  // Decimal Degrees
  if (coordinateFormat() == DD)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DecimalDegrees, 5);
    };
  }
  // Degrees Decimal Minutes
  else if (coordinateFormat() == DDM)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DegreesDecimalMinutes, 5);
    };
  }
  // UTM
  else if (coordinateFormat() == UTM)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUtm(p, UtmConversionMode::NorthSouthIndicators, true);
    };
  }
  // MGRS
  else if (coordinateFormat() == MGRS)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toMgrs(p, MgrsConversionMode::Automatic, 5, true);
    };
  }
  // USNG
  else if (coordinateFormat() == USNG)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUsng(p, 5, true);
    };
  }
  // GeoRef
  else if (coordinateFormat() == GeoRef)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toGeoRef(p, 5);
    };
  }
  // Gars
  else if (coordinateFormat() == Gars)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toGars(p);
    };
  }
  // DMS
  else
  {
    formatCoordinate =  [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DegreesMinutesSeconds, 3);
    };
  }
}

// Returns the current format to use
QString LocationTextController::coordinateFormat() const
{
  return m_coordinateFormat;
}

// Returns whether to use GPS for elevation. If false, it will use the Scene's Surface
bool LocationTextController::useGpsForElevation() const
{
  return m_useGpsForElevation;
}

// Sets whether to use GPS for elevation
void LocationTextController::setUseGpsForElevation(bool useGps)
{
  m_useGpsForElevation = useGps;
  emit propertyChanged(USE_GPS_PROPERTYNAME, useGps ? "true" : "false");
  emit useGpsForElevationChanged();
}

// Formats the elevation text for display in QML
void LocationTextController::formatElevationText(double elevation)
{
  if (unitOfMeasurement() == Feet)
  {
    elevation = elevation * 3.280839895;
  }
  m_currentElevationText = QString("%1 %2 MSL").arg(QString::number(elevation), unitOfMeasurement());
  emit currentElevationTextChanged();
}

// Returns the list of units
QStringList LocationTextController::units() const
{
  return m_units;
}

// Returns the current unit of measurement
QString LocationTextController::unitOfMeasurement() const
{
  return m_unitOfMeasurement;
}

// Sets the current unnit of measurement
void LocationTextController::setUnitOfMeasurement(const QString& unit)
{
  m_unitOfMeasurement = unit;
  emit propertyChanged(UNIT_OF_MEASUREMENT_PROPERTYNAME, unit);
  emit unitOfMeasurementChanged();
}
