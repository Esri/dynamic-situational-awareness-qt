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

#include "AbstractAlert.h"
#include "AlertToolController.h"
#include "AlertListModel.h"
#include "AlertListProxyModel.h"
#include "StatusAlertRule.h"

#include "ToolManager.h"

using namespace Esri::ArcGISRuntime;

AlertToolController::AlertToolController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_alertsProxyModel(new AlertListProxyModel(this)),
  m_statusAlertRule(new StatusAlertRule(this))
{
  Toolkit::ToolManager::instance().addTool(this);
}

AlertToolController::~AlertToolController()
{
}

QAbstractItemModel* AlertToolController::alertListModel() const
{
  return m_alertsProxyModel;
}

QString AlertToolController::toolName() const
{
  return "Alert Tool";
}

void AlertToolController::highlight(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AbstractAlert* alert = AlertListModel::instance()->alertAt(sourceIndex.row());

  if (!alert)
    return;

  alert->highlight();
}

void AlertToolController::setMinStatus(int status)
{
  AlertStatus alertStatus = static_cast<AlertStatus>(status);
  switch (alertStatus) {
  case AlertStatus::Low:
  case AlertStatus::Medium:
  case AlertStatus::High:
  case AlertStatus::Critical:
  case AlertStatus::Inactive:
    m_statusAlertRule->setMinStatus(alertStatus);
    m_alertsProxyModel->applyFilter(QList<AbstractAlertRule*>{m_statusAlertRule});
    break;
  default:
    break;
  }
}
