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

#include "ContactReportController.h"

// PCH header
#include "pch.hpp"

// example app headers
#include "AppConstants.h"
#include "Message.h"
#include "MessageFeedConstants.h"
#include "MessageSender.h"
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

ContactReportController::ContactReportController(QObject* parent):
  Toolkit::AbstractTool(parent),
  m_unitName(QHostInfo::localDomainName()),
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

  connect(this, &ContactReportController::activeChanged, this, &ContactReportController::onUpdateControlPointHightlight);
  connect(this, &ContactReportController::controlPointChanged, this, &ContactReportController::onUpdateControlPointHightlight);
}

ContactReportController::~ContactReportController()
{

}

QString ContactReportController::toolName() const
{
  return QStringLiteral("Contact Report");
}

void ContactReportController::setProperties(const QVariantMap& properties)
{
  auto findUserNameIt = properties.find(AppConstants::USERNAME_PROPERTYNAME);
  if (findUserNameIt != properties.end())
    setUnitName(findUserNameIt.value().toString());

  const auto contactReportConfig = properties[MessageFeedConstants::CONTACT_REPORT_CONFIG_PROPERTYNAME].toMap();
  auto findPortIt = contactReportConfig.find(MessageFeedConstants::CONTACT_REPORT_CONFIG_PORT);
  if (findPortIt != contactReportConfig.end())
  {
    bool ok = false;
    int newPort = findPortIt.value().toInt(&ok);
    if (ok)
      setUdpPort(newPort);
  }
}

QString ContactReportController::unitName() const
{
  return m_unitName;
}

QString ContactReportController::controlPoint() const
{
  if (!m_controlPointSet)
    return QString();

  return CoordinateFormatter::toLatitudeLongitude(m_controlPoint, LatitudeLongitudeFormat::DecimalDegrees, 3);
}

void ContactReportController::setUnitName(const QString& unitName)
{
  if (unitName == m_unitName)
    return;

  m_unitName = unitName;

  emit unitNameChanged();
}

void ContactReportController::setControlPoint(const Point& controlPoint)
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

bool ContactReportController::pickMode() const
{
  return m_pickMode;
}

void ContactReportController::setPickMode(bool pickMode)
{
  if (m_pickMode == pickMode)
    return;

  m_pickMode = pickMode;

  if (m_pickMode)
  {
    m_mouseClickConnection = connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked,
                                     this, &ContactReportController::onMouseClicked);
  }
  else
  {
    disconnect(m_mouseClickConnection);
  }

  emit pickModeChanged();
}

void ContactReportController::togglePickMode()
{
  disconnect(m_myLocationConnection);
  setPickMode(!m_pickMode);
}

void ContactReportController::setFromMyLocation()
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

void ContactReportController::sendReport(const QString& size,
                                         const QString& locationDescription,
                                         const QString& enemyUnitDescription,
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

  if (m_unitName.isEmpty() || m_controlPoint.isEmpty())
    return;

  if (observedTime.isNull() || !observedTime.isValid())
    return;

  if (size.isEmpty() || enemyUnitDescription.isEmpty())
    return;

  Message contactReport = Message(Message::MessageAction::Update, m_controlPoint);
  contactReport.setMessageId(QUuid::createUuid().toString());
  contactReport.setMessageType(QStringLiteral("spotrep"));

  QVariantMap attribs;
  attribs.insert(QStringLiteral("_control_points"), controlPoint());
  attribs.insert(QStringLiteral("datetimesubmitted"), QDateTime::currentDateTime().toString(Qt::ISODate));
  attribs.insert(QStringLiteral("uniquedesignation"), unitName());
  attribs.insert(QStringLiteral("equipment"), equipment);
  attribs.insert(QStringLiteral("activity"), activity);
  attribs.insert(QStringLiteral("location"), locationDescription);
  attribs.insert(QStringLiteral("size"), size);
  attribs.insert(QStringLiteral("timeobserved"), observedTime.toString(Qt::ISODate));
  attribs.insert(QStringLiteral("unit"), enemyUnitDescription);
  contactReport.setAttributes(attribs);

  if (!m_messageSender)
  {
    m_messageSender = new MessageSender(this);

    QUdpSocket* udpSocket = new QUdpSocket(m_messageSender);
    udpSocket->connectToHost(QHostAddress::Broadcast, m_udpPort, QIODevice::WriteOnly);
    m_messageSender->setDevice(udpSocket);
  }

  m_messageSender->sendMessage(contactReport.toGeoMessage());
}

void ContactReportController::cancelReport()
{
  m_controlPointSet = false;
  onUpdateControlPointHightlight();
  if (pickMode())
    togglePickMode();
}

int ContactReportController::udpPort() const
{
  return m_udpPort;
}

void ContactReportController::setUdpPort(int port)
{
  if (port == m_udpPort)
    return;

  m_udpPort = port;
}

void ContactReportController::onGeoViewChanged(GeoView* geoView)
{
  if (m_geoView == geoView)
    return;

  m_geoView = geoView;
}

void ContactReportController::onMouseClicked(QMouseEvent& event)
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

void ContactReportController::onUpdateControlPointHightlight()
{
  if (isActive() && m_controlPointSet)
    m_highlighter->startHighlight();
  else
    m_highlighter->stopHighlight();
}
