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

#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QObject>
#include <QUrl>

namespace Esri {
namespace ArcGISRuntime
{
  class Point;
}}

class QGeoPositionInfoSource;
class QCompass;
class GPSSimulator;

class LocationController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(bool simulated READ simulated WRITE setSimulated NOTIFY simulatedChanged)
  Q_PROPERTY(QUrl gpxFilePath READ gpxFilePath WRITE setGpxFilePath NOTIFY gpxFilePathChanged)
  Q_PROPERTY(QUrl defaultFileSearchPath READ defaultFileSearchPath CONSTANT)

public:
  LocationController(QObject* parent = nullptr);
  ~LocationController();

  QString toolName() const /*override*/;

signals:
  void positionChanged(const Esri::ArcGISRuntime::Point& newPosition);
  void headingChanged(double newHeading);
  void gpxFilePathChanged();
  void enabledChanged();
  void simulatedChanged();

public:
  bool enabled() const;
  void setEnabled(bool enabled);

  bool simulated() const;
  void setSimulated(bool simulated);

  QUrl gpxFilePath() const;
  void setGpxFilePath(const QUrl& gpxFilePath);

  QUrl defaultFileSearchPath() const;

private:
  GPSSimulator* m_simulator = nullptr;
  QGeoPositionInfoSource* m_positionSource = nullptr;
  QCompass* m_compass = nullptr;
  bool m_enabled = false;
  bool m_simulated = false;
  QUrl m_gpxFilePath;
};

#endif // LOCATIONCONTROLLER_H
