// Copyright 2016 ESRI
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

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "ArcGISTiledLayer.h"

#include "Vehicle.h"

#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;

Vehicle::Vehicle(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

Vehicle::~Vehicle()
{
}

void Vehicle::componentComplete()
{
  QQuickItem::componentComplete();

  // read the local data path
  m_dataPath = QQmlProperty::read(this, "dataPath").toString();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the light grey canvas tile package
  TileCache* tileCache = new TileCache(m_dataPath + QStringLiteral("/LightGreyCanvas.tpk"), this);
  m_map = new Map(new Basemap(new ArcGISTiledLayer(tileCache, this), this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Set viewpoint to Monterey, CA
  m_mapView->setViewpointCenter(Point(-121.9, 36.6, SpatialReference::wgs84()), 1e5);
}
