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

#include "AbstractAlert.h"
#include "AlertListModel.h"
#include "ViewedAlertsController.h"

#include "ToolManager.h"

using namespace Esri::ArcGISRuntime;

ViewedAlertsController::ViewedAlertsController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  AlertListModel* model = AlertListModel::instance();
  if (model)
  {
    connect(model, &AlertListModel::dataChanged, this, &ViewedAlertsController::unviewedCountChanged);
    connect(model, &AlertListModel::rowsInserted, this, &ViewedAlertsController::unviewedCountChanged);
    connect(model, &AlertListModel::rowsRemoved, this, &ViewedAlertsController::unviewedCountChanged);
    emit unviewedCountChanged();
  }
}

QString ViewedAlertsController::toolName() const
{
  return QString("viewed alerts");
}

ViewedAlertsController::~ViewedAlertsController()
{
}

int ViewedAlertsController::unviewedCount() const
{
  AlertListModel* model = AlertListModel::instance();
  if (!model)
    return 0;

  int count = 0;
  const int alertsCount = model->rowCount();
  for (int i = 0; i < alertsCount; ++i)
  {
    AbstractAlert* alert = model->alertAt(i);
    if (!alert)
      continue;

    if (!alert->active())
      continue;

    if (alert->viewed())
      continue;

    count++;
  }

  return count;
}
