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

#ifndef VIEWEDALERTSCONTROLLER_H
#define VIEWEDALERTSCONTROLLER_H

#include <QObject>

#include <QAbstractListModel>

#include "AbstractTool.h"

class AlertListProxyModel;
class StatusAlertRule;

class ViewedAlertsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(int unviewedCount READ unviewedCount NOTIFY unviewedCountChanged)

public:
  explicit ViewedAlertsController(QObject* parent = nullptr);
  ~ViewedAlertsController();

  int unviewedCount() const;

  // AbstractTool interface
  QString toolName() const;

signals:
  void unviewedCountChanged();
};

#endif // VIEWEDALERTSCONTROLLER_H
