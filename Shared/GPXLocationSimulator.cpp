// Copyright 2018 ESRI
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

// PCH header
#include "pch.hpp"

#include "GPXLocationSimulator.h"

// Qt headers
#include <QTimer>
#include <QXmlStreamReader>

// STL headers
#include <cmath>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class GPXLocationSimulator
  \inherits QGeoPositionInfoSource
  \brief Position source simulator that reads from a GPX file.
 */

/*!
  \brief Constructor taking an optional \a parent.

  To use simulation user must set GPX file the update interval
 */
GPXLocationSimulator::GPXLocationSimulator(QObject* parent) :
  QGeoPositionInfoSource(parent),
  m_gpxReader(new QXmlStreamReader()),
  m_timer(new QTimer(this)),
  m_angleOffset(0.0, -90.0, 0.0, 90.0)
{
  connectSignals();
  setUpdateInterval(500);
}

/*!
  \brief Constructor which populates the necessary components to run a GPS simulation.

  \list
    \li \a gpxFileName - The path of the GPX file containing position data.
    \li \a updateInterval - The update interval in milliseconds for position updates.
    \li \a parent - An optional parent.
  \endlist
 */
GPXLocationSimulator::GPXLocationSimulator(const QString& gpxFileName, int updateInterval, QObject* parent) :
  QGeoPositionInfoSource(parent),
  m_gpxReader(new QXmlStreamReader()),
  m_timer(new QTimer(this))
{
  connectSignals();
  setUpdateInterval(updateInterval);

  if (!setGpxFile(gpxFileName))
  {
    // raise error
    m_gpxFile.setFileName("");
  }
}

/*!
  \brief Destructor.
 */
GPXLocationSimulator::~GPXLocationSimulator()
{
}

/*!
  \internal
 */
void GPXLocationSimulator::connectSignals()
{
  connect(m_timer, SIGNAL(timeout()), this, SLOT(handleTimerEvent()));
  // internally we emit errorInternal but we cannot emit error due to syntax
  connect(this, &GPXLocationSimulator::errorInternal,
          this, static_cast<void (QGeoPositionInfoSource::*)(QGeoPositionInfoSource::Error)>(&QGeoPositionInfoSource::error));
}

/*!
  \internal
 */
bool GPXLocationSimulator::gotoNextPositionElement()
{
  while (!m_gpxReader->atEnd() && !m_gpxReader->hasError())
  {
    if (m_gpxReader->isStartElement())
    {
      if (m_gpxReader->name().compare(QString("trkpt"), Qt::CaseInsensitive) == 0)
      {
        return true;
      }
    }

    m_gpxReader->readNext();
  }

  return false;
}

/*!
  \internal
 */
Point GPXLocationSimulator::getNextPoint(QTime& time)
{
  // Convert the current gpx position to Point and QTime parmeters.
  if (!gotoNextPositionElement())
  {
    return Point();
  }

  // fetch the lat and lon attributes from the trkpt element
  const QXmlStreamAttributes attrs = m_gpxReader->attributes();
  const double x = attrs.value("lon").toString().toDouble();
  const double y = attrs.value("lat").toString().toDouble();
  double z = NAN;
  // if the new point is the same as the old point then trash it and try to get another.
  if (x == m_latestPoint.x() && y == m_latestPoint.y())
  {
    m_gpxReader->readNext();
    return getNextPoint(time);
  }

  // goto the start of the time child element
  m_gpxReader->readNextStartElement();

  while (m_gpxReader->name().compare(QString("trkpt"), Qt::CaseInsensitive) != 0 && !m_gpxReader->atEnd())
  {
    if (m_gpxReader->isStartElement())
    {
      if (m_gpxReader->name().compare(QString("ele"), Qt::CaseInsensitive) == 0)
      {
        z = m_gpxReader->readElementText().toDouble();
      }
      else if (m_gpxReader->name().compare(QString("time"), Qt::CaseInsensitive) == 0)
      {
        const QString timeString = m_gpxReader->readElementText();
        const int hours = timeString.section(":", 0, 0).right(2).toInt();
        const int minutes = timeString.section(":", 1, 1).toInt();
        const int seconds = timeString.section(":", 2, 2).left(2).toInt();
        time.setHMS(hours ,minutes, seconds, 0);
      }
    }

    m_gpxReader->readNext();
  }

  m_latestPoint = std::isnan(z) ?
        Point(x, y, SpatialReference::wgs84()) :
        Point(x, y, z, SpatialReference::wgs84());

  return m_latestPoint;
}

/*!
  \brief Starts position updates.

  Loads a GPX file into a stream reader, fetches the first 3 coordinates
  and starts a timer that performs interpolation and position updating.
 */
void GPXLocationSimulator::startUpdates()
{
  if (isStarted())
    return;

  // if the gpx file does not contain enough information to
  // interpolate on then cancel the simulation.
  if (!initializeInterpolationValues())
  {
    return;
  }

  // start the position update timer
  m_timer->start(updateInterval());
  m_isStarted = true;
}

/*!
  \brief Requests an update.
 */
void GPXLocationSimulator::requestUpdate(int)
{
  Q_UNIMPLEMENTED();
}

/*!
  \brief Stops updates.
 */
void GPXLocationSimulator::stopUpdates()
{
  m_timer->stop();
  m_isStarted = false;
}

/*!
  \brief Returns whether the simulation is active.
 */
bool GPXLocationSimulator::isActive()
{
  return m_timer->isActive();
}

/*!
  \brief Returns whether the simulation is started.
 */
bool GPXLocationSimulator::isStarted()
{
  return m_isStarted;
}

/*!
  \brief \brief Returns the last known position.

  The parameter \a fromSatellitePositioningMethodsOnly is ignored.
 */
QGeoPositionInfo GPXLocationSimulator::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
  Q_UNUSED(fromSatellitePositioningMethodsOnly)
  return m_lastKnownPosition;
}

/*!
  \brief Returns the supported positioning methods.

  For this source this will be \c QGeoPositionInfoSource::PositioningMethod::NoPositioningMethods.
 */
QGeoPositionInfoSource::PositioningMethods GPXLocationSimulator::supportedPositioningMethods() const
{
  return QGeoPositionInfoSource::PositioningMethod::NoPositioningMethods;
}

/*!
  \brief Returns the QGeoPositionInfoSource::Error.
 */
QGeoPositionInfoSource::Error GPXLocationSimulator::error() const
{
  return m_lastError;
}

/*!
  \brief Returns the minimum update interval in milliseconds.
 */
int GPXLocationSimulator::minimumUpdateInterval() const
{
  return updateInterval();
}

/*!
  \internal

 increments the current time
 fetches new positions from the gpx file as necessary
 calculates and sets the current position and orientation
 */
void GPXLocationSimulator::handleTimerEvent()
{
  // update the current time
  m_currentTime = m_currentTime.addMSecs(m_timer->interval() * m_playbackMultiplier);

  // determine if a new position needs to be obtained from the gpx
  if (m_currentTime > m_segmentEndTime)
  {
    if (!updateInterpolationParameters())
    {
      m_gpxReader->clear();
      m_gpxReader->addData(m_gpxData);
      initializeInterpolationValues();

      return;
    }
  }

  // normalize the time across the current segment
  const double val1 = static_cast<double>(m_segmentStartTime.msecsTo(m_currentTime));
  const double val2 = static_cast<double>(m_segmentStartTime.msecsTo(m_segmentEndTime));
  const double normalizedTime = val1 / val2;

  // get the interpolated position and orientation on the current
  // segment based on the normalized time.
  const Point currentPosition = normalizedTime <= 0.5 ? m_currentSegment.startPoint() : m_currentSegment.endPoint();
  const double currentHeading = getInterpolatedHeading(currentPosition, normalizedTime);

  QGeoPositionInfo qtPosition;
  auto timeStamp = QDateTime::currentDateTime();
  timeStamp.setTime(m_currentTime);
  qtPosition.setTimestamp(timeStamp);

  qtPosition.setCoordinate(QGeoCoordinate(currentPosition.y(), currentPosition.x(), currentPosition.z()));

  m_lastKnownPosition = qtPosition;
  emit positionUpdated(qtPosition);
  emit headingChanged(currentHeading);
}

/*!
  \internal
 */
bool GPXLocationSimulator::initializeInterpolationValues()
{
  // fetch the first 3 points from the gpx feed to populate the
  // initial interpolation components.
  const Point pt1 = getNextPoint(m_segmentStartTime);
  const Point pt2 = getNextPoint(m_segmentEndTime);
  const Point pt3 = getNextPoint(m_nextSegmentEndTime);

  if (pt1.isEmpty() || pt2.isEmpty() || pt3.isEmpty())
  {
    return false;
  }

  // define the interpolation segments
  m_currentSegment = LineSegment(pt1, pt2, SpatialReference::wgs84());
  m_nextSegment = LineSegment(pt2, pt3, SpatialReference::wgs84());
  m_startHeadingDelta = 0;
  m_endHeadingDelta = heading(m_currentSegment);

  // define the current time as the first timestamp extracted from the gpx file
  m_currentTime = m_segmentStartTime;

  return true;
}

/*!
  \internal
 */
double GPXLocationSimulator::getInterpolatedHeading(const Point& currentPosition, double normalizedTime)
{
  // implementation for smooth orientation transfer between segments.
  // the smoothing is spread across the final 10% of the current segment
  // and the first 10% of the next segment.

  LineSegment segment;

  // interpolation of the first 10% of the segment
  if (normalizedTime < 0.1)
  {
    segment = LineSegment(m_currentSegment.startPoint(), currentPosition, SpatialReference::wgs84());
    return heading(m_currentSegment);
  }
  // interpolation of the last 10% of the segment
  else if (normalizedTime > 0.9)
  {
    segment = LineSegment(m_currentSegment.endPoint(), currentPosition, SpatialReference::wgs84());
    return heading(m_currentSegment);
  }

  // no orientation interpolation needed, use the current segments angle
  return heading(m_currentSegment);
}

/*!
  \internal
 */
bool GPXLocationSimulator::updateInterpolationParameters()
{
  // fetch the next coordinate in the gpx file and updates all the
  // internal interpolation vars

  m_segmentStartTime = m_segmentEndTime;
  m_segmentEndTime = m_nextSegmentEndTime;
  Point newPt = getNextPoint(m_nextSegmentEndTime);

  // if there are no more points to get then notify simulation to start over
  if (newPt.isEmpty())
  {
    return false;
  }

  // discard the oldest segment and populate the newest segment.
  m_currentSegment = m_nextSegment;
  m_nextSegment = LineSegment(m_currentSegment.endPoint(), newPt, SpatialReference::wgs84());

  // discard the oldest orientation delta and populate the newest
  m_startHeadingDelta = m_endHeadingDelta;
  m_endHeadingDelta = heading(m_currentSegment);

  return true;
}

/*!
  \brief Returns the GPX file location.
 */
QString GPXLocationSimulator::gpxFile()
{
  return m_gpxFile.fileName();
}

/*!
  \brief Sets the GPX file location to \a fileName.

  Returns whether the file was succesfully read.
 */
bool GPXLocationSimulator::setGpxFile(const QString& fileName)
{
  if (!QFile::exists(fileName))
  {
    m_lastError = QGeoPositionInfoSource::Error::AccessError;
    emit this->errorInternal(m_lastError);
    return false;
  }

  if (m_gpxFile.isOpen())
    m_gpxFile.close();

  m_gpxFile.setFileName(fileName);

  if (!m_gpxFile.open(QFile::ReadOnly | QFile::Text))
    return false;

  m_gpxData = m_gpxFile.readAll();
  m_gpxReader->clear();
  m_gpxReader->addData(m_gpxData);
  m_gpxFile.close();

  m_isStarted = false;

  return true;
}

/*!
  \brief Returns the playback multiplier.
 */
int GPXLocationSimulator::playbackMultiplier()
{
  return m_playbackMultiplier;
}

/*!
  \brief Sets the playback multiplier to \a val.

  Used if GPX timestamps are either too close or two far
 */
void GPXLocationSimulator::setPlaybackMultiplier(int val)
{
  m_playbackMultiplier = val;
}

/*!
  \brief Returns the heading in degrees of the supplied \a segment.
 */
double GPXLocationSimulator::heading(const Esri::ArcGISRuntime::LineSegment& segment) const
{
  const auto startPoint = segment.startPoint();
  const auto endPoint = segment.endPoint();
  return m_angleOffset.angleTo(QLineF(startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y()));
}

} // Dsa
