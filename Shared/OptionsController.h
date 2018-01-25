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

#ifndef OPTIONSCONTROLLER_H
#define OPTIONSCONTROLLER_H

class LocationTextController;

#include "AbstractTool.h"
#include <QObject>

class OptionsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QStringList coordinateFormats READ coordinateFormats NOTIFY coordinateFormatsChanged)
  Q_PROPERTY(QStringList units READ units NOTIFY unitsChanged)
  Q_PROPERTY(bool useGpsForElevation READ useGpsForElevation WRITE setUseGpsForElevation NOTIFY useGpsForElevationChanged)

public:
  explicit OptionsController(QObject* parent = nullptr);
  ~OptionsController();

  QString toolName() const override;
  Q_INVOKABLE void setCoordinateFormat(const QString& format);
  Q_INVOKABLE void setUnitOfMeasurement(const QString& unit);

signals:
  void coordinateFormatsChanged();
  void useGpsForElevationChanged();
  void unitsChanged();

private:
  LocationTextController* m_locationTextController = nullptr;
  QStringList m_coordinateFormats;
  bool m_useGpsForElevation = false;
  QStringList m_units;

  void getUpdatedTools();
  QStringList coordinateFormats() const;
  QStringList units() const;
  bool useGpsForElevation() const;
  void setUseGpsForElevation(bool useGps);
};

#endif // OPTIONSCONTROLLER_H
