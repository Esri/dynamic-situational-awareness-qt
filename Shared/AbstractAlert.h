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

#ifndef ABSTRACTALERT_H
#define ABSTRACTALERT_H

#include "Geometry.h"

#include <QObject>
#include <QString>
#include <QUuid>

enum class AlertStatus : unsigned int
{
  Inactive = 0,
  Low,
  Medium,
  High,
  Critical
};

Q_DECLARE_METATYPE(AlertStatus)

class AbstractAlert : public QObject
{
  Q_OBJECT

public:
  explicit AbstractAlert(QObject* parent = nullptr);
  ~AbstractAlert();

  void registerAlert();

  AlertStatus status() const;
  void setStatus(const AlertStatus& status);

  virtual Esri::ArcGISRuntime::Geometry position() const = 0;

  virtual void highlight() = 0;

  QString message() const;
  void setMessage(const QString &message);

  QUuid id() const;
  void setId(const QUuid& id);

signals:
  void statusChanged();

private:
  AlertStatus m_status = AlertStatus::Inactive;
  QString m_message;
  QUuid m_id;
};

#endif // ABSTRACTALERT_H
