/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

#ifndef COORDINATECONVERSIONOPTIONS_H
#define COORDINATECONVERSIONOPTIONS_H

#include <QObject>
#include <QStringList>

#include "ToolkitCommon.h"
#include "GeometryTypes.h"

namespace Dsa
{

class TOOLKIT_EXPORT CoordinateConversionOptions : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(CoordinateType outputMode READ outputMode WRITE setOutputMode NOTIFY outputModeChanged)
  Q_PROPERTY(bool addSpaces READ addSpaces WRITE setAddSpaces NOTIFY addSpacesChanged)
  Q_PROPERTY(int precision READ precision WRITE setPrecision NOTIFY precisionChanged)
  Q_PROPERTY(int decimalPlaces READ decimalPlaces WRITE setDecimalPlaces NOTIFY decimalPlacesChanged)

  Q_PROPERTY(Esri::ArcGISRuntime::MgrsConversionMode mgrsConversionMode READ mgrsConversionMode WRITE setMgrsConversionMode NOTIFY mgrsConversionModeChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::LatitudeLongitudeFormat latLonFormat READ latLonFormat WRITE setLatLonFormat NOTIFY latLonFormatChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::UtmConversionMode utmConversionMode READ utmConversionMode WRITE setUtmConversionMode NOTIFY utmConversionModeChanged)

  Q_PROPERTY(QStringList coordinateTypeNames READ coordinateTypeNames CONSTANT)

public:
  enum CoordinateType
  {
    CoordinateTypeGars,
    CoordinateTypeGeoRef,
    CoordinateTypeLatLon,
    CoordinateTypeMgrs,
    CoordinateTypeUsng,
    CoordinateTypeUtm
  };

  // static qml methods
  Q_INVOKABLE CoordinateType stringToCoordinateType(const QString& type) const;
  Q_INVOKABLE QString coordinateTypeToString(CoordinateType type) const;
  Q_INVOKABLE QStringList coordinateTypeNames() const;

signals:
  void nameChanged();
  void outputModeChanged();
  void addSpacesChanged();
  void precisionChanged();
  void decimalPlacesChanged();
  void mgrsConversionModeChanged();
  void latLonFormatChanged();
  void utmConversionModeChanged();
  void garsConversionModeChanged();

public:
  CoordinateConversionOptions(QObject* parent = nullptr);
  ~CoordinateConversionOptions() override;

  QString name() const;
  void setName(const QString& name);

  CoordinateType outputMode() const;
  void setOutputMode(CoordinateType outputMode);

  bool addSpaces() const;
  void setAddSpaces(bool addSpaces);

  int precision() const;
  void setPrecision(int precision);

  int decimalPlaces() const;
  void setDecimalPlaces(int decimalPlaces);

  Esri::ArcGISRuntime::MgrsConversionMode mgrsConversionMode() const;
  void setMgrsConversionMode(Esri::ArcGISRuntime::MgrsConversionMode mgrsConversionMode);

  Esri::ArcGISRuntime::LatitudeLongitudeFormat latLonFormat() const;
  void setLatLonFormat(Esri::ArcGISRuntime::LatitudeLongitudeFormat latLonFormat);

  Esri::ArcGISRuntime::UtmConversionMode utmConversionMode() const;
  void setUtmConversionMode(Esri::ArcGISRuntime::UtmConversionMode utmConversionMode);

  Esri::ArcGISRuntime::GarsConversionMode garsConvesrionMode() const;
  void setGarsConversionMode(Esri::ArcGISRuntime::GarsConversionMode conversionMode);

private:
  QString m_name;
  CoordinateType m_outputMode = CoordinateTypeUsng;
  bool m_addSpaces = true;
  int m_precision = 8;
  int m_decimalPlaces = 6;

  Esri::ArcGISRuntime::MgrsConversionMode m_mgrsConversionMode = Esri::ArcGISRuntime::MgrsConversionMode::Automatic;
  Esri::ArcGISRuntime::LatitudeLongitudeFormat m_latLonFormat = Esri::ArcGISRuntime::LatitudeLongitudeFormat::DecimalDegrees;
  Esri::ArcGISRuntime::UtmConversionMode m_utmConversionMode = Esri::ArcGISRuntime::UtmConversionMode::LatitudeBandIndicators;
  Esri::ArcGISRuntime::GarsConversionMode m_garsConvesrionMode = Esri::ArcGISRuntime::GarsConversionMode::Center;
};

} // Dsa

Q_DECLARE_METATYPE(Dsa::CoordinateConversionOptions::CoordinateType)

#endif // COORDINATECONVERSIONOPTIONS_H
