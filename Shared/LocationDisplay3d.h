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

#ifndef LOCATIONDISPLAY3D_H
#define LOCATIONDISPLAY3D_H

// C++ API headers
#include "Point.h"

// Qt headers
#include <QObject>

namespace Esri {
namespace ArcGISRuntime {
  class Graphic;
  class GraphicsOverlay;
  class Symbol;
  class SimpleRenderer;
}}

class QGeoPositionInfoSource;
class QCompass;

namespace Dsa {

class LocationDisplay3d : public QObject
{
  Q_OBJECT

public:
  explicit LocationDisplay3d(QObject* parent = nullptr);
  ~LocationDisplay3d();

  void start();
  void stop();
  bool isStarted() const;

  void setZOffset(double offset);
  double zOffset();

  QGeoPositionInfoSource* positionSource() const;
  void setPositionSource(QGeoPositionInfoSource* positionSource);

  QCompass* compass() const;
  void setCompass(QCompass* compass);

  Esri::ArcGISRuntime::GraphicsOverlay* locationOverlay() const;

  Esri::ArcGISRuntime::Graphic* locationGraphic() const;

  Esri::ArcGISRuntime::Symbol* defaultSymbol() const;
  void setDefaultSymbol(Esri::ArcGISRuntime::Symbol* defaultSymbol);

signals:
  void locationChanged(const Esri::ArcGISRuntime::Point& location);
  void headingChanged();

private:
  Q_DISABLE_COPY(LocationDisplay3d)

  void postLastKnownLocationUpdate();

  mutable Esri::ArcGISRuntime::GraphicsOverlay* m_locationOverlay = nullptr;
  Esri::ArcGISRuntime::SimpleRenderer* m_locationRenderer = nullptr;
  Esri::ArcGISRuntime::Graphic* m_locationGraphic = nullptr;
  Esri::ArcGISRuntime::Symbol* m_defaultSymbol = nullptr;
  QGeoPositionInfoSource* m_geoPositionInfoSource = nullptr;
  QCompass* m_compass = nullptr;
  Esri::ArcGISRuntime::Point m_lastKnownLocation;
  bool m_isStarted = false;
  double m_zOffset = 10.0;

  QMetaObject::Connection m_positionErrorConnection;
  QMetaObject::Connection m_positionUpdateConnection;
  QMetaObject::Connection m_headingConnection;
};

} // Dsa

#endif // LOCATIONDISPLAY3D_H
