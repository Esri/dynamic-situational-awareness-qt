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

#include "DsaUtility.h"

// C++ API headers
#include "SpatialReference.h"

// Qt headers
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QtMath>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::DsaUtility
  \inmodule Dsa
  \brief Static helper class with a variety of utility methods for the Dsa app.
 */

/*!
  \brief Returns the platform independent data path to \c [HOME]/ArcGIS/Runtime/Data/DSA.
 */
QString DsaUtility::dataPath()
{
  QDir dataDir;
#ifdef Q_OS_ANDROID
  dataDir = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
#elif defined Q_OS_IOS
  dataDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
#else
  dataDir = QDir::home();
#endif

  dataDir.cd("ArcGIS");
  dataDir.cd("Runtime");
  dataDir.cd("Data");
  dataDir.cd("DSA");

  return dataDir.exists() ? dataDir.absolutePath() : "";
}

/*!
  \brief Returns an \l Esri::ArcGISRuntime::Point in Monterey, California.
 */
Point DsaUtility::montereyCA()
{
  return Point(-121.9, 36.6, SpatialReference::wgs84());
}

/*!
  \brief Returns the distance in meters between the \a from and \a to points.

  \note Assumes both points are in the same spatial reference.
 */
double DsaUtility::distance3D(const Point& from, const Point& to)
{
  const QVector3D fromCartesian = toCartesianPoint(from);
  const QVector3D toCartesian = toCartesianPoint(to);

  const QVector3D result(fromCartesian - toCartesian);

  return result.length();
}

/*!
  \brief Returns \a point as a Cartesian point.
 */
QVector3D DsaUtility::toCartesianPoint(const Point& point)
{
  // convert degrees (lat/long) to radians
  constexpr double degreesToRadians = M_PI/180.0;
  const double xRadians = point.x() * degreesToRadians;
  const double yRadians = point.y() * degreesToRadians;

  // convert to cartesian coords
  constexpr double earthRadius = 6378137.0;
  const double radius = point.z() + earthRadius;
  const double radCosLat = radius * std::cos(yRadians);

  return QVector3D(radCosLat * std::sin(xRadians), radius * std::sin(yRadians), radCosLat * std::cos(xRadians));
}

} // Dsa
