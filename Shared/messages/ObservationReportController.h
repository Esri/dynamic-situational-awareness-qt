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

#ifndef OBSERVATIONREPORTCONTROLLER_H
#define OBSERVATIONREPORTCONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// C++ API headers
#include "Point.h"

class QDateTime;
class QMouseEvent;

namespace Esri {
namespace ArcGISRuntime {
  class GeoView;
}
}

namespace Dsa {

class DataSender;

class PointHighlighter;

class ObservationReportController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
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
