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

#include "LocationTextController.h"

// C++ API headers
#include "CoordinateFormatter.h"
#include "ErrorException.h"
#include "Scene.h"
#include "Surface.h"

// Qt headers
#include <QFuture>

// DSA headers
#include "AddLocalDataController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

// constant strings used for properties in the config file
const QString LocationTextController::COORDINATE_FORMAT_PROPERTYNAME = QStringLiteral("CoordinateFormat");
const QString LocationTextController::USE_GPS_PROPERTYNAME = QStringLiteral("UseGpsForElevation");
const QString LocationTextController::UNIT_OF_MEASUREMENT_PROPERTYNAME = QStringLiteral("UnitOfMeasurement");

// constant strings for formats/units
const QString LocationTextController::DMS = QStringLiteral("DMS");
const QString LocationTextController::DD = QStringLiteral("DD");
const QString LocationTextController::DDM = QStringLiteral("DDM");
const QString LocationTextController::UTM = QStringLiteral("UTM");
const QString LocationTextController::MGRS = QStringLiteral("MGRS");
const QString LocationTextController::USNG = QStringLiteral("USNG");
const QString LocationTextController::GeoRef = QStringLiteral("GEOREF");
const QString LocationTextController::Gars = QStringLiteral("GARS");
const QString LocationTextController::Meters = QStringLiteral("meters");
const QString LocationTextController::Feet = QStringLiteral("feet");

/*!
  \class Dsa::LocationTextController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for displaying the current location.
 */

/*!
 \brief Constructor that takes an optional \a parent.
 */
LocationTextController::LocationTextController(QObject* parent) :
  AbstractTool(parent),
  m_coordinateFormat(DMS),
  m_unitOfMeasurement(Meters)
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged,
          this, &LocationTextController::onGeoViewChanged);

  connect(ToolResourceProvider::instance(), &ToolResourceProvider::locationChanged,
          this, &LocationTextController::onLocationChanged);

  m_conToolAdded = connect(&ToolManager::instance(), &ToolManager::toolAdded, this, &LocationTextController::onToolAdded);

  ToolManager::instance().addTool(this);
}

/*!
 \brief Destructor.
 */
LocationTextController::~LocationTextController()
{
}

/*!
 \brief Returns the Tool Name.
 */
QString LocationTextController::toolName() const
{
  return QStringLiteral("Location Text");
}

/*!
 \property LocationTextController::currentLocationText
 \brief Returns the location text string for display in QML.
 */
QString LocationTextController::currentLocationText() const
{
  return m_currentLocationText;
}

/*!
 \property LocationTextController::currentElevationText
 \brief Returns the elevation text string for display in QML.
 */
QString LocationTextController::currentElevationText() const
{
  return m_currentElevationText;
}

/*!
 \brief Slot for ToolResourceProvider::locationChanged.

 Uses the provided \a pt to update the location and elevation text.
 */
void LocationTextController::onLocationChanged(const Point& pt)
{
  if (m_coordinateFormat.isEmpty())
    return;

  if (formatCoordinate == nullptr)
    return;

  // update location text
  m_currentLocationText = QString("%1 (%2)").arg(formatCoordinate(pt), m_coordinateFormat);
  emit currentLocationTextChanged();

  // update the elevation text
  if (m_useGpsForElevation)
  {
    formatElevationText(pt.z());
    return;
  }

  // abort if there's no valid elevation surface
  if (!m_surface)
    return;

  // call the elevation function and pass the result to the format method
  m_surface->elevationAsync(pt).then(this, [this](double elevation)
  {
    formatElevationText(elevation);
  }).onFailed(this, [this](const ErrorException& e)
  {
    Q_UNUSED(e);
    // multiple failures likely indicate a failed loading of the
    // elevation source for the surface. set the member to nullptr
    // to prevent continued calls for fetching the elevation
    m_elevationFailures++;
    if (m_elevationFailures > 2)
      m_surface = nullptr;
  });
}

void LocationTextController::onToolAdded(AbstractTool* newTool)
{
  // skip if the tool pointed to is not valid
  if (!newTool)
    return;

  // catch only the addition of the AddLocalDataController tool
  if (auto* addlocalDataController = qobject_cast<AddLocalDataController*>(newTool); addlocalDataController)
  {
    // create a connection to the signal for when an elevation source is selected/changed
    connect(addlocalDataController, &AddLocalDataController::elevationSourceSelected, this, [this](ElevationSource* source)
    {
      Q_UNUSED(source);
      resetSurface();
    });

    // stop listening for new tools being added
    disconnect(m_conToolAdded);
  }
}

/*!
 \brief Slot to obtain the Scene's base surface.
 */
void LocationTextController::onGeoViewChanged()
{
  resetSurface();
}

/*!
 \brief Set \a properties from the configuration file.
 */
void LocationTextController::setProperties(const QVariantMap& properties)
{
  setCoordinateFormat(properties[COORDINATE_FORMAT_PROPERTYNAME].toString());
  setUseGpsForElevation(properties[USE_GPS_PROPERTYNAME].toBool());
  setUnitOfMeasurement(properties[UNIT_OF_MEASUREMENT_PROPERTYNAME].toString());
}

/*!
 \brief Changes the coordinate \a format.
 */
void LocationTextController::setCoordinateFormat(const QString& format)
{
  if (format == m_coordinateFormat && formatCoordinate != nullptr)
    return;

  m_coordinateFormat = format;
  emit propertyChanged(COORDINATE_FORMAT_PROPERTYNAME, format);
  emit coordinateFormatChanged();

  // use std::function to change the lambda that the formatCoordinate member points to
  // Decimal Degrees
  const QString currentFormat = coordinateFormat();
  if (currentFormat == DD)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DecimalDegrees, 5);
    };
  }
  // Degrees Decimal Minutes
  else if (currentFormat == DDM)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DegreesDecimalMinutes, 5);
    };
  }
  // UTM
  else if (currentFormat == UTM)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUtm(p, UtmConversionMode::NorthSouthIndicators, true);
    };
  }
  // MGRS
  else if (currentFormat == MGRS)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toMgrs(p, MgrsConversionMode::Automatic, 5, true);
    };
  }
  // USNG
  else if (currentFormat == USNG)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toUsng(p, 5, true);
    };
  }
  // GEOREF
  else if (currentFormat == GeoRef)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toGeoRef(p, 5);
    };
  }
  // GARS
  else if (currentFormat == Gars)
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toGars(p);
    };
  }
  // DMS
  else
  {
    formatCoordinate = [](const Point& p)
    {
      return CoordinateFormatter::toLatitudeLongitude(p, LatitudeLongitudeFormat::DegreesMinutesSeconds, 3);
    };
  }
}

/*!
 \brief Returns the current format to use.
 */
QString LocationTextController::coordinateFormat() const
{
  return m_coordinateFormat;
}

/*!
  \brief Returns whether to use GPS for elevation.

  If false, it will use the Scene's Surface.
 */
bool LocationTextController::useGpsForElevation() const
{
  return m_useGpsForElevation;
}

/*!
 \brief Set whether to use GPS for elevation to \a useGps.
 */
void LocationTextController::setUseGpsForElevation(bool useGps)
{
  if (useGps == m_useGpsForElevation)
    return;

  m_useGpsForElevation = useGps;
  emit propertyChanged(USE_GPS_PROPERTYNAME, useGps ? "true" : "false");
  emit useGpsForElevationChanged();
}

/*!
 \brief Formats the \a elevation text for display in QML.
*/
void LocationTextController::formatElevationText(double elevation)
{
  if (unitOfMeasurement() == Feet)
  {
    elevation = elevation * 3.280839895;
  }
  m_currentElevationText = QString("%1 %2 MSL").arg(QString::number(elevation), unitOfMeasurement());
  emit currentElevationTextChanged();
}

void LocationTextController::resetSurface()
{
  // update the current surface pointer
  if (const auto* scene = ToolResourceProvider::instance()->scene(); scene)
  {
    m_elevationFailures = 0;
    m_surface = scene->baseSurface();
  }
}

/*!
 \brief Returns the current unit of measurement.
*/
QString LocationTextController::unitOfMeasurement() const
{
  return m_unitOfMeasurement;
}

/*!
 \brief Sets the current \a unit of measurement.
 */
void LocationTextController::setUnitOfMeasurement(const QString& unit)
{
  if (unit == m_unitOfMeasurement)
    return;

  m_unitOfMeasurement = unit;
  emit propertyChanged(UNIT_OF_MEASUREMENT_PROPERTYNAME, unit);
  emit unitOfMeasurementChanged();
}

} // Dsa

// Signal Documentation
/*!
  \fn void LocationTextController::currentLocationTextChanged();
  \brief Signal emitted when the current location text changes.
 */

/*!
  \fn void LocationTextController::currentElevationTextChanged();
  \brief Signal emitted when the current elevation text changes.
 */

/*!
  \fn void LocationTextController::useGpsForElevationChanged();
  \brief Signal emitted when the \l useGpsForElevtion changes.

  If \c true, GPS is used for elevation. If \c false, the Surface is used
  for elevation.
 */

/*!
  \fn void LocationTextController::unitOfMeasurementChanged();
  \brief Signal emitted when the unit of measurement changes.
 */

/*!
  \fn void LocationTextController::coordinateFormatChanged();
  \brief Signal emitted when the coordinate format changes.
 */

