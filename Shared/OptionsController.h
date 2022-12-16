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

#ifndef OPTIONSCONTROLLER_H
#define OPTIONSCONTROLLER_H

// dsa headers
#include "AbstractTool.h"

// Qt headers
#include <QObject>

namespace Esri::ArcGISRuntime {
class DictionaryRenderer;
}

namespace Dsa {

class LocationTextController;

class OptionsController : public AbstractTool
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

} // Dsa

#endif // OPTIONSCONTROLLER_H
