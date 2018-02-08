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

#ifndef LOCATIONDISPLAY3D_H
#define LOCATIONDISPLAY3D_H

#include <QObject>

#include "Point.h"

namespace Esri {
namespace ArcGISRuntime
{
  class Graphic;
  class GraphicsOverlay;
  class Symbol;
  class SimpleRenderer;
}}

class QGeoPositionInfoSource;
class QCompass;

class LocationDisplay3d : public QObject
{
  Q_OBJECT

public:
  explicit LocationDisplay3d(QObject* parent = nullptr);
  ~LocationDisplay3d();

  void start();
  void stop();
  bool isStarted() const;

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

  QMetaObject::Connection m_positionErrorConnection;
  QMetaObject::Connection m_positionUpdateConnection;
  QMetaObject::Connection m_headingConnection;
};

#endif // LOCATIONDISPLAY3D_H
