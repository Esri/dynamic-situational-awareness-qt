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

#ifndef CONTACTREPORTCONTROLLER_H
#define CONTACTREPORTCONTROLLER_H


// toolkit headers
#include "AbstractTool.h"

// C++ API headers
#include "Point.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class GeoView;
}
}

class MessageSender;

class QDateTime;
class QMouseEvent;

class ContactReportController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QString unitName READ unitName WRITE setUnitName NOTIFY unitNameChanged)
  Q_PROPERTY(QString controlPoint READ controlPoint NOTIFY controlPointChanged)
  Q_PROPERTY(bool pickMode READ pickMode WRITE setPickMode NOTIFY pickModeChanged)

public:

  explicit ContactReportController(QObject* parent = nullptr);
  ~ContactReportController();

  QString toolName() const override;

  void setProperties(const QVariantMap& properties) override;

  QString unitName() const;
  void setUnitName(const QString& unitName);

  QString controlPoint() const;

  bool pickMode() const;
  void setPickMode(bool pickMode);

  Q_INVOKABLE void togglePickMode();

  Q_INVOKABLE void sendReport(const QString& size,
                              const QString& locationDescription,
                              const QString& enemyUnitDescription,
                              const QDateTime& observedTime,
                              const QString& equipment);

  int udpPort() const;
  void setUdpPort(int port);

signals:
  void unitNameChanged();
  void controlPointChanged();
  void pickModeChanged();

public slots:
  void onGeoViewChanged(Esri::ArcGISRuntime::GeoView* geoView);

private slots:
  void onMouseClicked(QMouseEvent& event);

private:
  void setControlPoint(const Esri::ArcGISRuntime::Point& controlPoint);

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  QString m_unitName;
  Esri::ArcGISRuntime::Point m_controlPoint;
  MessageSender* m_messageSender = nullptr;
  int m_udpPort = 45679;
  bool m_pickMode = false;

  QMetaObject::Connection m_mouseClickConnection;
};

#endif // CONTACTREPORTCONTROLLER_H
