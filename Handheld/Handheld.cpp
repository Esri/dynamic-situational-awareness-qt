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
#include "CoordinateConversionController.h"
#include "CoordinateConversionOptions.h"

#include "Handheld.h"

#include <QQmlProperty>

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Solutions;
using CCO = CoordinateConversionOptions;

Handheld::Handheld(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

Handheld::~Handheld()
{
}

void Handheld::componentComplete()
{
  QQuickItem::componentComplete();

  // read the local data path
  m_dataPath = QQmlProperty::read(this, "dataPath").toString();

  // find QML Coordinate Conversion Controller object
  m_coordinateConversionController = findChild<CoordinateConversionController*>("coordinateConversionController");

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");
  m_mapView->setWrapAroundMode(WrapAroundMode::Disabled);

  // Create a map using the light grey canvas tile package
  TileCache* tileCache = new TileCache(m_dataPath + QStringLiteral("/LightGreyCanvas.tpk"), this);
  m_map = new Map(new Basemap(new ArcGISTiledLayer(tileCache, this), this), this);

  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& mouseEvent)
  {
    const auto point = m_mapView->screenToLocation(mouseEvent.x(), mouseEvent.y());
    m_coordinateConversionController->setPointToConvert(point);
  });

  connect(m_map, &Map::doneLoading, this, [this](Error)
  {
    m_coordinateConversionController->setSpatialReference(m_map->spatialReference());
  });

  // Set map to map view
  m_mapView->setMap(m_map);

  // Set viewpoint to Monterey, CA
  m_mapView->setViewpointCenter(Point(-121.9, 36.6, SpatialReference::wgs84()), 1e5);

  // set the options for the coordinateConversionTool
  setCoordinateConversionOptions();
}

void DSA_Vehicle::setCoordinateConversionOptions()
{
  if (!m_coordinateConversionController)
    return;

  CoordinateConversionOptions* option = new CoordinateConversionOptions(this);
  option->setName("Coast Gaurd");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesDecimalMinutes);

  m_coordinateConversionController->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("Air Force");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUsng);
  option->setPrecision(7);
  option->setAddSpaces(true);

  m_coordinateConversionController->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("Squad Alpha");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUtm);
  option->setUtmConversionMode(CCO::UtmConversionMode::UtmConversionModeNorthSouthIndicators);
  option->setAddSpaces(true);

  m_coordinateConversionController->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("Division B");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesMinutesSeconds);
  option->setDecimalPlaces(12);

  m_coordinateConversionController->addOption(option);
}
