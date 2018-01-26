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

#include "LocationAlertSource.h"

#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

LocationAlertSource::LocationAlertSource(QObject* parent):
  AlertSource(parent)
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged, this, [this](const Point& location)
  {
    m_location = location;
    emit positionChanged();
  });
}

LocationAlertSource::~LocationAlertSource()
{

}

Point LocationAlertSource::position() const
{
  return m_location;
}
