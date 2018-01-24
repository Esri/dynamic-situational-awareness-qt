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

#ifndef ALERTCONDITIONDATA_H
#define ALERTCONDITIONDATA_H

#include "AlertLevel.h"

#include "Geometry.h"

#include <QObject>
#include <QString>
#include <QUuid>

class AlertCondition;

namespace Esri
{
namespace ArcGISRuntime
{
  class GeoElement;
}
}

class AlertConditionData : public QObject
{
  Q_OBJECT

public:
  explicit AlertConditionData(AlertCondition* condition);
  ~AlertConditionData();

  AlertLevel level() const;

  virtual Esri::ArcGISRuntime::Geometry position() const = 0;

  virtual void highlight(bool on) = 0;

  virtual Esri::ArcGISRuntime::GeoElement* geoElement() const = 0;

  QString name() const;

  QUuid id() const;
  void setId(const QUuid& id);

  bool viewed() const;
  void setViewed(bool viewed);

  bool active() const;
  void setActive(bool active);

public slots:
  void onPositionChanged();

signals:
  void statusChanged();
  void viewedChanged();
  void positionChanged();
  void activeChanged();
  void noLongerValid();

private:
  AlertCondition* m_condition = nullptr;
  QUuid m_id;
  bool m_viewed = false;
  bool m_active = false;
};

#endif // ALERTCONDITIONDATA_H
