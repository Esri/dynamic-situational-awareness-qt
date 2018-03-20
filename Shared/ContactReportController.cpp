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

// C++ API headers
#include "CoordinateFormatter.h"

// Qt headers
#include <QHostInfo>

using namespace Esri::ArcGISRuntime;

ContactReportController::ContactReportController(QObject* parent):
  Toolkit::AbstractTool(parent),
  m_unitName(QHostInfo::localDomainName())
{
  Toolkit::ToolManager::instance().addTool(this);
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
