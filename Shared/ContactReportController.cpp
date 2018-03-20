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

using namespace Esri::ArcGISRuntime;

ContactReportController::ContactReportController(QObject* parent):
  Toolkit::AbstractTool(parent),
  m_unitName(QHostInfo::localDomainName())
{
  Toolkit::ToolManager::instance().addTool(this);

  // connect to ToolResourceProvider signals
  auto resourecProvider = Toolkit::ToolResourceProvider::instance();
  connect(resourecProvider, &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]()
  {
    onGeoViewChanged(Toolkit::ToolResourceProvider::instance()->geoView());
  });
  onGeoViewChanged(resourecProvider->geoView());
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

  auto findPortIt = properties.find(QStringLiteral("port"));
  if (findPortIt != properties.end())
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

  emit controlPointChanged();
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
  setPickMode(!m_pickMode);
}

void ContactReportController::sendReport(const QString& size,
                                         const QString& locationDescription,
                                         const QString& enemyUnitDescription,
                                         const QDateTime& observedTime,
                                         const QString& equipment)
{
  if (m_unitName.isEmpty() || m_controlPoint.isEmpty())
    return;

  if (observedTime.isNull() || !observedTime.isValid())
    return;

  if (size.isEmpty() || enemyUnitDescription.isEmpty())
    return;
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
