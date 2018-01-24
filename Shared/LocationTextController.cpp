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

#include "LocationTextController.h"

#include "CoordinateFormatter.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

LocationTextController::LocationTextController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged, this, [this](const Point& pt)
  {
    updateLocationText(pt);
  });
}

LocationTextController::~LocationTextController()
{
}

QString LocationTextController::toolName() const
{
  return QStringLiteral("location text");
}

QString LocationTextController::currentLocationText() const
{
  return m_currentLocationText;
}

QString LocationTextController::currentElevationText() const
{
  return m_currentElevationText;
}

void LocationTextController::updateLocationText(const Point& pt)
{
  m_currentLocationText = CoordinateFormatter::toLatitudeLongitude(pt, LatitudeLongitudeFormat::DegreesMinutesSeconds, 8);
  m_currentElevationText = QString::number(pt.z());
  emit currentLocationTextChanged();
  emit currentElevationTextChanged();
}
