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

#ifndef OBSERVATIONREPORTCONTROLLER_H
#define OBSERVATIONREPORTCONTROLLER_H

// dsa headers
#include "AbstractTool.h"

class QDateTime;
class QMouseEvent;

namespace Esri::ArcGISRuntime {
  class GeoView;
  class Point;
}

namespace Dsa {

class DataSender;

class PointHighlighter;

class ObservationReportController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QString observedBy READ observedBy WRITE setObservedBy NOTIFY observedByChanged)
  Q_PROPERTY(QString controlPoint READ controlPoint NOTIFY controlPointChanged)
  Q_PROPERTY(bool pickMode READ pickMode WRITE setPickMode NOTIFY pickModeChanged)

public:

  explicit ObservationReportController(QObject* parent = nullptr);
  ~ObservationReportController();

  QString toolName() const override;

  void setProperties(const QVariantMap& properties) override;

  QString observedBy() const;
  void setObservedBy(const QString& observedBy);

  QString controlPoint() const;

  bool pickMode() const;
  void setPickMode(bool pickMode);

  Q_INVOKABLE void togglePickMode();

  Q_INVOKABLE void setFromMyLocation();

  Q_INVOKABLE void broadcastReport(const QString& size,
                              const QString& locationDescription,
                              const QString& description,
                              const QString& activity,
                              const QDateTime& observedTime,
                              const QString& equipment);

  Q_INVOKABLE void cancelReport();

  void setControlPoint(const Esri::ArcGISRuntime::Point& controlPoint);

  int udpPort() const;
  void setUdpPort(int port);

signals:
  void observedByChanged();
  void controlPointChanged();
  void pickModeChanged();

public slots:
  void onGeoViewChanged(Esri::ArcGISRuntime::GeoView* geoView);

private slots:
  void onMouseClicked(QMouseEvent& event);
  void onUpdateControlPointHightlight();

private:

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  QString m_observedBy;
  Esri::ArcGISRuntime::Point m_controlPoint;
  DataSender* m_dataSender = nullptr;
  PointHighlighter* m_highlighter = nullptr;
  bool m_controlPointSet = false;
  int m_udpPort = -1;
  bool m_pickMode = false;

  QMetaObject::Connection m_mouseClickConnection;
  QMetaObject::Connection m_myLocationConnection;
};

} // Dsa

#endif // OBSERVATIONREPORTCONTROLLER_H
