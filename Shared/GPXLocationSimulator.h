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

#ifndef GPSSIMULATOR_H
#define GPSSIMULATOR_H

#include <QFile>
#include <QLineF>
#include <QTime>
#include <QPointF>
#include <QGeoPositionInfoSource>
#include <memory>
#include "Point.h"
#include "LineSegment.h"

class QXmlStreamReader;
class QTimer;
class GPXLocationSimulator : public QGeoPositionInfoSource
{
  Q_OBJECT

public:
  explicit GPXLocationSimulator(QObject* parent = nullptr);
  GPXLocationSimulator(const QString& gpxFileName, int updateInterval = 20, QObject* parent = nullptr);
  ~GPXLocationSimulator();

  QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
  PositioningMethods supportedPositioningMethods() const override;
  int minimumUpdateInterval() const override;

  bool isActive();
  bool isStarted();

  QString gpxFile();
  bool setGpxFile(const QString& fileName);

  int timerInterval();
  void setTimerInterval(int milliseconds);

  int playbackMultiplier();
  void setPlaybackMultiplier(int multiplier);

  QGeoPositionInfoSource::Error error() const override;

public slots:
  void startUpdates() override;
  void stopUpdates() override;
  void requestUpdate(int timeout = 0) override;

signals:
  void positionUpdateAvailable(const Esri::ArcGISRuntime::Point& pos, double heading);

  // This is only used internally. Use QGeoPositionInfoSource::error signal instead.
  void errorInternal(QGeoPositionInfoSource::Error);

private:
  void pauseSimulation();
  void resumeSimulation();

  bool gotoNextPositionElement();
  Esri::ArcGISRuntime::Point getNextPoint(QTime& time);
  bool updateInterpolationParameters();
  bool initializeInterpolationValues();
  double heading(const Esri::ArcGISRuntime::LineSegment& segment) const;

  double getInterpolatedHeading(const Esri::ArcGISRuntime::Point& currentPosition, double normalizedTime);

  QFile m_gpxFile;
  QByteArray m_gpxData;
  std::unique_ptr<QXmlStreamReader> m_gpxReader;
  QTimer* m_timer = nullptr;
  int m_playbackMultiplier = 1;
  Esri::ArcGISRuntime::LineSegment m_currentSegment;
  Esri::ArcGISRuntime::LineSegment m_nextSegment;
  Esri::ArcGISRuntime::Point m_latestPoint;
  double m_startHeadingDelta;
  double m_endHeadingDelta;
  QTime m_currentTime;
  QTime m_segmentStartTime;
  QTime m_segmentEndTime;
  QTime m_nextSegmentEndTime;
  bool m_isStarted = false;
  const QLineF m_angleOffset;
  QGeoPositionInfo m_lastKnownPosition;
  QGeoPositionInfoSource::Error m_lastError = QGeoPositionInfoSource::NoError;

private slots:
  void handleTimerEvent();
};

#endif // GPSSIMULATOR_H
