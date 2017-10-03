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

#include "SceneQuickView.h"

#include "DsaController.h"
#include "Vehicle.h"
#include "CoordinateConversionController.h"

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;
using CCO = CoordinateConversionOptions;

Vehicle::Vehicle(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_controller(new DsaController(this))
{
}

Vehicle::~Vehicle()
{
}

void Vehicle::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  connect(m_sceneView, &SceneQuickView::errorOccurred, m_controller, &DsaController::onError);

  m_controller->init(m_sceneView);

  // Set scene to scene view
  m_sceneView->setArcGISScene(m_controller->scene());

  // set the options for the coordinateConversionTool
  setCoordinateConversionOptions();
}

void Vehicle::setCoordinateConversionOptions()
{
  CoordinateConversionController* controller = findChild<CoordinateConversionController*>("coordinateConversionController");

  if (!controller)
    return;

  CoordinateConversionOptions* option = new CoordinateConversionOptions(this);
  option->setName("DDM");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesDecimalMinutes);

  controller->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("USNG");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUsng);
  option->setPrecision(7);
  option->setAddSpaces(true);

  controller->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("UTM");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeUtm);
  option->setUtmConversionMode(CCO::UtmConversionMode::UtmConversionModeNorthSouthIndicators);
  option->setAddSpaces(true);

  controller->addOption(option);

  option = new CoordinateConversionOptions(this);
  option->setName("DMS");
  option->setOutputMode(CCO::CoordinateType::CoordinateTypeLatLon);
  option->setLatLonFormat(CCO::LatitudeLongitudeFormat::LatitudeLongitudeFormatDegreesMinutesSeconds);
  option->setDecimalPlaces(12);

  controller->addOption(option);
}
