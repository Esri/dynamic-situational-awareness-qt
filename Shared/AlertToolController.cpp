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
#include "AlertToolController.h"
#include "AlertListModel.h"
#include "AlertListProxyModel.h"
#include "DistanceAlertRule.h"
#include "DsaUtility.h"
#include "IdsAlertRule.h"
#include "StatusAlertRule.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "GeoView.h"
#include "SceneView.h"

using namespace Esri::ArcGISRuntime;

AlertToolController::AlertToolController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_alertsProxyModel(new AlertListProxyModel(this)),
  m_distanceAlertRule(new DistanceAlertRule(this)),
  m_statusAlertRule(new StatusAlertRule(this)),
  m_idsAlertRule(new IdsAlertRule(this))
{
  Toolkit::ToolManager::instance().addTool(this);
  m_rules.append(m_distanceAlertRule);
  m_rules.append(m_statusAlertRule);
  m_rules.append(m_idsAlertRule);

  m_alertsProxyModel->applyFilter(m_rules);
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

void AlertToolController::zoomTo(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AbstractAlert* alert = AlertListModel::instance()->alertAt(sourceIndex.row());
  if (!alert)
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  const Point pos = alert->position().extent().center();

  SceneView* sceneView = dynamic_cast<SceneView*>(geoView);

  if (sceneView)
  {
    const Camera currentCam = sceneView->currentViewpointCamera();
    Camera newCam = currentCam.zoomToward(pos, 10.);

    sceneView->setViewpointCamera(newCam, 1.);
  }
  else
  {
    const Viewpoint currVP = geoView->currentViewpoint(ViewpointType::CenterAndScale);
    const Viewpoint newViewPoint(pos, currVP.targetScale());

    geoView->setViewpoint(newViewPoint, 1.);
  }
}

void AlertToolController::setViewed(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AlertListModel* model = AlertListModel::instance();
  if (!model)
    return;

  model->setData(sourceIndex, QVariant::fromValue(true), AlertListModel::AlertListRoles::Viewed);
}

void AlertToolController::dismiss(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AbstractAlert* alert = AlertListModel::instance()->alertAt(sourceIndex.row());
  if (!alert)
    return;

  m_idsAlertRule->addId(alert->id());
  m_alertsProxyModel->applyFilter(m_rules);
}

void AlertToolController::setMinStatus(int status)
{
  AlertStatus alertStatus = static_cast<AlertStatus>(status);
  switch (alertStatus) {
  case AlertStatus::Low:
  case AlertStatus::Medium:
  case AlertStatus::High:
  case AlertStatus::Critical:
    m_statusAlertRule->setMinStatus(alertStatus);
    m_alertsProxyModel->applyFilter(m_rules);
    break;
  default:
    break;
  }
}
