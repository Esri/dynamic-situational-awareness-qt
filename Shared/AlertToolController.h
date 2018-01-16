// Copyright 2016 ESRI
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

#ifndef ALERTTOOLCONTROLLER_H
#define ALERTTOOLCONTROLLER_H

#include <QObject>

#include <QAbstractListModel>

#include "AbstractTool.h"

class AbstractAlertRule;
class AlertListProxyModel;
class DistanceAlertRule;
class IdsAlertRule;
class StatusAlertRule;

class AlertToolController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* alertListModel READ alertListModel NOTIFY alertListModelChanged)

public:
  explicit AlertToolController(QObject* parent = nullptr);
  ~AlertToolController();

  QAbstractItemModel* alertListModel() const;

  // AbstractTool interface
  QString toolName() const override;

  Q_INVOKABLE void highlight(int rowIndex);
  Q_INVOKABLE void zoomTo(int rowIndex);
  Q_INVOKABLE void setViewed(int rowIndex);
  Q_INVOKABLE void dismiss(int rowIndex);
  Q_INVOKABLE void setMinStatus(int status);

signals:
  void alertListModelChanged();

private:
  AlertListProxyModel* m_alertsProxyModel;
  DistanceAlertRule* m_distanceAlertRule;
  StatusAlertRule* m_statusAlertRule;
  IdsAlertRule* m_idsAlertRule;
  QList<AbstractAlertRule*> m_rules;
};

#endif // ALERTTOOLCONTROLLER_H
