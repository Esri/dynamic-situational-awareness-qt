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


// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QObject>

namespace Esri {
namespace ArcGISRuntime {
class DictionaryRenderer;
}
}

class LocationTextController;


class OptionsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QStringList coordinateFormats READ coordinateFormats NOTIFY coordinateFormatsChanged)
  Q_PROPERTY(QStringList units READ units NOTIFY unitsChanged)
  Q_PROPERTY(bool useGpsForElevation READ useGpsForElevation WRITE setUseGpsForElevation NOTIFY useGpsForElevationChanged)
  Q_PROPERTY(int initialFormatIndex READ initialFormatIndex NOTIFY initialFormatIndexChanged)
  Q_PROPERTY(int initialUnitIndex READ initialUnitIndex NOTIFY initialUnitIndexChanged)
  Q_PROPERTY(bool showFriendlyTracksLabels READ showFriendlyTracksLabels WRITE setShowFriendlyTracksLabels NOTIFY showFriendlyTracksLabelsChanged)
  Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)

public:
  explicit OptionsController(QObject* parent = nullptr);
  ~OptionsController();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;
  Q_INVOKABLE void setCoordinateFormat(const QString& format);
  Q_INVOKABLE void setUnitOfMeasurement(const QString& unit);

  QString userName() const;
  void setUserName(const QString &userName);

signals:
  void coordinateFormatsChanged();
  void useGpsForElevationChanged();
  void unitsChanged();
  void initialUnitIndexChanged();
  void initialFormatIndexChanged();
  void showFriendlyTracksLabelsChanged();
  void userNameChanged();

private:
  static const QString COORDINATE_FORMAT_PROPERTYNAME;
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

  LocationTextController* m_locationTextController = nullptr;
  int m_initialUnitIndex;
  int m_initialFormatIndex;
  QString m_unitOfMeasurement;
  QString m_coordinateFormat;
  QStringList m_coordinateFormatOptions;
  QStringList m_units;
  QString m_userName;

  void getUpdatedTools();
  QStringList coordinateFormats() const;
  QStringList units() const;
  bool useGpsForElevation() const;
  void setUseGpsForElevation(bool useGps);
  int initialUnitIndex() const;
  int initialFormatIndex() const;
  bool showFriendlyTracksLabels();
  void setShowFriendlyTracksLabels(bool show);
  QList<Esri::ArcGISRuntime::DictionaryRenderer*> friendlyTracksOverlayRenderers() const;
};

#endif // OPTIONSCONTROLLER_H
