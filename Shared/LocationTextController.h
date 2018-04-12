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

#ifndef LOCATIONTEXTCONTROLLER_H
#define LOCATIONTEXTCONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

namespace Esri {
namespace ArcGISRuntime {
class Point;
class Surface;
}
}

namespace Dsa {

class LocationTextController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QString currentLocationText READ currentLocationText NOTIFY currentLocationTextChanged)
  Q_PROPERTY(QString currentElevationText READ currentElevationText NOTIFY currentElevationTextChanged)

public:
  explicit LocationTextController(QObject* parent = nullptr);
  ~LocationTextController();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;
  void setCoordinateFormat(const QString& format);
  QString coordinateFormat() const;
  void setUnitOfMeasurement(const QString& unit);
  QString unitOfMeasurement() const;
  bool useGpsForElevation() const;
  void setUseGpsForElevation(bool useGps);

signals:
  void currentLocationTextChanged();
  void currentElevationTextChanged();
  void useGpsForElevationChanged();
  void unitOfMeasurementChanged();
  void coordinateFormatChanged();

private slots:
  void onGeoViewChanged();
  void onLocationChanged(const Esri::ArcGISRuntime::Point& pt);

private:
  std::function<QString(const Esri::ArcGISRuntime::Point&)> formatCoordinate;

  QString currentLocationText() const;
  QString currentElevationText() const;
  void formatElevationText(double elevation);

  static const QString COORDINATE_FORMAT_PROPERTYNAME;
  static const QString USE_GPS_PROPERTYNAME;
  static const QString UNIT_OF_MEASUREMENT_PROPERTYNAME;
  static const QString DMS;
  static const QString DD;
  static const QString DDM;
  static const QString UTM;
  static const QString MGRS;
  static const QString USNG;
  static const QString GeoRef;
  static const QString Gars;
  static const QString Meters;
  static const QString Feet;

  Esri::ArcGISRuntime::Surface* m_surface = nullptr;
  QString m_currentLocationText = "Location Unavailable";
  QString m_currentElevationText = "Elevation Unavailable";
  QString m_coordinateFormat;
  bool m_useGpsForElevation = false;
  QString m_unitOfMeasurement;
};

} // Dsa

#endif // LOCATIONTEXTCONTROLLER_H
