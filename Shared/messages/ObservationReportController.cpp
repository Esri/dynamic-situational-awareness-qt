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

#include "ObservationReportController.h"

// example app headers
#include "AppConstants.h"
#include "DataSender.h"
#include "Message.h"
#include "MessageFeedConstants.h"
#include "PointHighlighter.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "CoordinateFormatter.h"
#include "GeoView.h"
#include "MapView.h"
#include "SceneView.h"

// Qt headers
#include <QDateTime>
#include <QHostInfo>
#include <QUdpSocket>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::ObservationReportController
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Tool controller for creating observation reports.
 */

/*!
  \brief Constructor accepting an optional \a parent.
 */
ObservationReportController::ObservationReportController(QObject* parent):
  Toolkit::AbstractTool(parent),
  m_observedBy(QHostInfo::localHostName()),
  m_highlighter(new PointHighlighter(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  // connect to ToolResourceProvider signals
  auto resourceProvider = Toolkit::ToolResourceProvider::instance();
  connect(resourceProvider, &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]()
  {
    onGeoViewChanged(Toolkit::ToolResourceProvider::instance()->geoView());
  });
  onGeoViewChanged(resourceProvider->geoView());

  connect(this, &ObservationReportController::activeChanged, this, &ObservationReportController::onUpdateControlPointHightlight);
  connect(this, &ObservationReportController::controlPointChanged, this, &ObservationReportController::onUpdateControlPointHightlight);
}

/*!
  \brief Destructor.
 */
ObservationReportController::~ObservationReportController()
{

}

/*!
  \brief Returns the name of this tool - \c "Observation Report".
 */
QString ObservationReportController::toolName() const
{
  return QStringLiteral("Observation Report");
}

/*! \brief Sets any values in \a properties which are relevant for the observation report controller.
 *
 * This tool will use the following key/value pairs in the \a properties map if they are set:
 *
 * \list
 *  \li \c ObservationReportConfig. A JSON object describing options for the observation report including
 * the \c port.
 *  \li \c UserName. The user name (observed by) for observation reports.
 * \endlist
 */
void ObservationReportController::setProperties(const QVariantMap& properties)
{
  auto findUserNameIt = properties.find(AppConstants::USERNAME_PROPERTYNAME);
  if (findUserNameIt != properties.end())
    setObservedBy(findUserNameIt.value().toString());

  const auto observationReportConfig = properties[MessageFeedConstants::OBSERVATION_REPORT_CONFIG_PROPERTYNAME].toMap();
  auto findPortIt = observationReportConfig.find(MessageFeedConstants::OBSERVATION_REPORT_CONFIG_PORT);
  if (findPortIt != observationReportConfig.end())
  {
    bool ok = false;
    int newPort = findPortIt.value().toInt(&ok);
    if (ok)
      setUdpPort(newPort);
  }
}

/*!
  \property ObservationReportController::observedBy
  \brief Returns the name of the unit making the observation report.
 */
QString ObservationReportController::observedBy() const
{
  return m_observedBy;
}

/*!
  \property ObservationReportController::controlPoint
  \brief Returns the control point location of the observation report in decimal degrees.
 */
QString ObservationReportController::controlPoint() const
{
  if (!m_controlPointSet)
    return QString();

  return CoordinateFormatter::toLatitudeLongitude(m_controlPoint, LatitudeLongitudeFormat::DecimalDegrees, 3);
}

/*!
  \brief Sets the name of the unit making this observation report to \a unitName.
 */
void ObservationReportController::setObservedBy(const QString& observedBy)
{
  if (observedBy == m_observedBy)
    return;

  m_observedBy = observedBy;

  emit observedByChanged();
}

/*!
  \brief Sets the control point location for this observation report to \a controlPoint.
 */
void ObservationReportController::setControlPoint(const Point& controlPoint)
{
  if (m_controlPoint == controlPoint)
    return;

  m_controlPoint = controlPoint;
  m_controlPointSet = true;

  emit controlPointChanged();

  m_highlighter->stopHighlight();
  m_highlighter->onPointChanged(m_controlPoint);
  onUpdateControlPointHightlight();
}

/*!
  \property ObservationReportController::pickMode
  \brief Returns whether the tool is in pick mode. If \c true,
  the tool will use clicks in the geoView to update the \l controlPoint.
 */
bool ObservationReportController::pickMode() const
{
  return m_pickMode;
}

/*!
  \brief Sets the pick mode of the tool to \a pickMode.
 */
void ObservationReportController::setPickMode(bool pickMode)
{
  if (m_pickMode == pickMode)
    return;

  m_pickMode = pickMode;

  if (m_pickMode)
  {
    m_mouseClickConnection = connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked,
                                     this, &ObservationReportController::onMouseClicked);
  }
  else
  {
    disconnect(m_mouseClickConnection);
  }

  emit pickModeChanged();
}

/*!
  \brief Toggle the current state of \l pickMode.
 */
void ObservationReportController::togglePickMode()
{
  disconnect(m_myLocationConnection);
  setPickMode(!m_pickMode);
}

/*!
  \brief Sets the control point location for the observation report to the current
  location of the app.

  \note The location will be set when the next location update is received.
 */
void ObservationReportController::setFromMyLocation()
{
  if (pickMode())
    togglePickMode();

  auto resourceProvider = Toolkit::ToolResourceProvider::instance();

  m_myLocationConnection = connect(resourceProvider, &Toolkit::ToolResourceProvider::locationChanged, this, [this](const Point& location)
  {
    disconnect(m_myLocationConnection);
    setControlPoint(location);
  });
}

/*!
  \brief Broadcasts the observation report over the current \l udpPort.

  The report will include the attributes:
  \list
    \li \a size Size of object observed or number of items.
    \li \a locationDescription. The location of the observation.
    \li \a description. Description of who is performing activity (can be ‘Unknown’).
    \li \a activity. The activity observed.
    \li \a observedTime. Date and time of observation.
    \li \a equipment. Equipment of unit observed.
  \endlist

  In addition, the report will include the following pre-defined attributes:
  \list
    \li \l controlPoint. The control point location of the observation in decimal degrees.
    \li Date and time submitted. The system time that this method was called.
    \li \l unitName. The name/unique designation of the unit making the report.
  \endlist

 */
void ObservationReportController::broadcastReport(const QString& size,
                                         const QString& locationDescription,
                                         const QString& description,
                                         const QString& activity,
                                         const QDateTime& observedTime,
                                         const QString& equipment)
{
  m_controlPointSet = false;
  onUpdateControlPointHightlight();
  if (pickMode())
    togglePickMode();

  if (m_udpPort == -1)
    return;

  if (m_observedBy.isEmpty() || m_controlPoint.isEmpty())
    return;

  if (observedTime.isNull() || !observedTime.isValid())
    return;

  if (size.isEmpty() || description.isEmpty())
    return;

  Message observationReport = Message(Message::MessageAction::Update, m_controlPoint);
  observationReport.setMessageId(QUuid::createUuid().toString());
  observationReport.setMessageType(QStringLiteral("spotrep"));

  QVariantMap attribs;
  attribs.insert(QStringLiteral("_control_points"), controlPoint());
  attribs.insert(QStringLiteral("datetimesubmitted"), QDateTime::currentDateTime().toString(Qt::ISODate));
  attribs.insert(QStringLiteral("uniquedesignation"), m_observedBy);
  attribs.insert(QStringLiteral("equipment"), equipment);
  attribs.insert(QStringLiteral("activity"), activity);
  attribs.insert(QStringLiteral("location"), locationDescription);
  attribs.insert(QStringLiteral("size"), size);
  attribs.insert(QStringLiteral("timeobserved"), observedTime.toString(Qt::ISODate));
  attribs.insert(QStringLiteral("unit"), description);
  observationReport.setAttributes(attribs);

  if (!m_dataSender)
  {
    m_dataSender = new DataSender(this);

    QUdpSocket* udpSocket = new QUdpSocket(m_dataSender);
    udpSocket->connectToHost(QHostAddress::Broadcast, m_udpPort, QIODevice::WriteOnly);
    m_dataSender->setDevice(udpSocket);
  }

  m_dataSender->sendData(observationReport.toGeoMessage());
}

/*!
  \brief Cancels the current report.
 */
void ObservationReportController::cancelReport()
{
  m_controlPointSet = false;
  onUpdateControlPointHightlight();
  if (pickMode())
    togglePickMode();
}

/*!
  \brief Returns the UDP port over which to broadcast observation reports.
 */
int ObservationReportController::udpPort() const
{
  return m_udpPort;
}

/*!
  \brief Sets the UDP port over which to broadcast observation reports to \a port.
 */
void ObservationReportController::setUdpPort(int port)
{
  if (port == m_udpPort)
    return;

  m_udpPort = port;
}

/*!
  \brief Sets the geoView to be used by the tool to \a geoView.
 */
void ObservationReportController::onGeoViewChanged(GeoView* geoView)
{
  if (m_geoView == geoView)
    return;

  m_geoView = geoView;
}

/*!
  \internal
 */
void ObservationReportController::onMouseClicked(QMouseEvent& event)
{
  if (!isActive())
    return;

  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  if (!m_pickMode)
    return;

  if (!m_geoView)
    return;

  togglePickMode();

  SceneView* sceneView = dynamic_cast<SceneView*>(m_geoView);
  if (sceneView)
  {
    setControlPoint(sceneView->screenToBaseSurface(event.x(), event.y()));
  }
  else
  {
    MapView* mapView = dynamic_cast<MapView*>(m_geoView);
    if (mapView)
      setControlPoint(mapView->screenToLocation(event.x(), event.y()));
    else
      return;
  }

  event.accept();
}

/*!
  \internal
 */
void ObservationReportController::onUpdateControlPointHightlight()
{
  if (isActive() && m_controlPointSet)
    m_highlighter->startHighlight();
  else
    m_highlighter->stopHighlight();
}

} // Dsa

// Signal Documentation
/*!
  \fn void ObservationReportController::observedByChanged();
  \brief Signal emitted when the observedBy property changes.
 */

/*!
  \fn void ObservationReportController::controlPointChanged();
  \brief Signal emitted when the controlPoint property changes.
 */

/*!
  \fn void ObservationReportController::pickModeChanged();
  \brief Signal emitted when the pickMode property changes.
 */
