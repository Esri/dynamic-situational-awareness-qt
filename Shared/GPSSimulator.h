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
#include <memory>

class QXmlStreamReader;
class QTimer;
class GPSSimulator : public QObject
{
  Q_OBJECT

public:
  explicit GPSSimulator(QObject* parent = nullptr);
  GPSSimulator(const QString& fileName, int updateInterval = 20, QObject* parent = nullptr);
  ~GPSSimulator();

  void startSimulation();
  void pauseSimulation();
  void resumeSimulation();

  bool isActive();
  bool isStarted();

  QString gpxFile();
  bool setGpxFile(const QString& fileName);

  int timerInterval();
  void setTimerInterval(int milliseconds);

  int playbackMultiplier();
  void setPlaybackMultiplier(int multiplier);

signals:
  void positionUpdateAvailable(const QPointF& pos, double orientation);

private:
  bool gotoNextPositionElement();
  void getNextPoint(QPointF& point, QTime& time);
  bool updateInterpolationParameters();
  bool initializeInterpolationValues();

  double getInterpolatedOrientation(const QPointF& currentPosition, double normalizedTime);

  QFile m_gpxFile;
  QByteArray m_gpxData;
  std::unique_ptr<QXmlStreamReader> m_gpxReader;
  QTimer* m_timer = nullptr;
  int m_timerInterval = 20;
  int m_playbackMultiplier = 1;
  QLineF m_currentSegment;
  QLineF m_nextSegment;
  QPointF m_latestPoint;
  double m_startOrientationDelta;
  double m_endOrientationDelta;
  QTime m_currentTime;
  QTime m_segmentStartTime;
  QTime m_segmentEndTime;
  QTime m_nextSegmentEndTime;
  bool m_isStarted = false;

private slots:
  void handleTimerEvent();
};

#endif // GPSSIMULATOR_H
