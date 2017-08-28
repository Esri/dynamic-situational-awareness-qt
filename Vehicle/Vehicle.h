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

#ifndef VEHICLE_H
#define VEHICLE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Map;
class MapQuickView;
namespace Solutions
{
class CoordinateConversionController;
}
}
}

#include <QQuickItem>

class Vehicle : public QQuickItem
{
  Q_OBJECT

public:
  Vehicle(QQuickItem* parent = nullptr);
  ~Vehicle();

  void componentComplete() override;

private:
  void setCoordinateConversionOptions();

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::Solutions::CoordinateConversionController* m_coordinateConversionController = nullptr;

  QString m_dataPath;
};

#endif // VEHICLE_H
