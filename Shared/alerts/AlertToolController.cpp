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

#include "AlertConditionData.h"
#include "AlertToolController.h"
#include "AlertListModel.h"
#include "AlertListProxyModel.h"
#include "GeoElementHighlighter.h"
#include "WithinAreaAlertQuery.h"
#include "WithinDistanceAlertQuery.h"
#include "DsaUtility.h"
#include "IdsAlertRule.h"
#include "StatusAlertRule.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "GeoView.h"
#include "GraphicsOverlay.h"
#include "SceneView.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleRenderer.h"

#include <QTimer>

using namespace Esri::ArcGISRuntime;

AlertToolController::AlertToolController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_alertsProxyModel(new AlertListProxyModel(this)),
  m_distanceAlertRule(new WithinDistanceAlertQuery(this)),
  m_intersectsRule(new WithinAreaAlertQuery(this)),
  m_statusAlertRule(new StatusAlertRule(this)),
  m_idsAlertRule(new IdsAlertRule(this)),
  m_highlighter(new GeoElementHighlighter(this))
{
  Toolkit::ToolManager::instance().addTool(this);
  m_rules.append(m_distanceAlertRule);
  m_rules.append(m_intersectsRule);
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

void AlertToolController::highlight(int rowIndex, bool showHighlight)
{
  if (showHighlight)
  {
    QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
    AlertConditionData* alert = AlertListModel::instance()->alertAt(sourceIndex.row());

    if (!alert)
      return;

    if (m_highlightConnection)
      disconnect(m_highlightConnection);

    m_highlightConnection = connect(alert, &AlertConditionData::noLongerValid, this, [this]()
    {
      m_highlighter->stopHighlight();
    });

    m_highlighter->setGeoElement(alert->geoElement());
    m_highlighter->startHighlight();
  }
  else
  {
    m_highlighter->stopHighlight();
  }
}

void AlertToolController::zoomTo(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AlertConditionData* alert = AlertListModel::instance()->alertAt(sourceIndex.row());
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
    Camera newCam = currentCam.zoomToward(pos, 10.0);

    sceneView->setViewpointCamera(newCam, 1.0);
  }
  else
  {
    const Viewpoint currVP = geoView->currentViewpoint(ViewpointType::CenterAndScale);
    const Viewpoint newViewPoint(pos, currVP.targetScale());

    geoView->setViewpoint(newViewPoint, 1.0);
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
  AlertConditionData* alert = AlertListModel::instance()->alertAt(sourceIndex.row());
  if (!alert)
    return;

  m_idsAlertRule->addId(alert->id());
  m_alertsProxyModel->applyFilter(m_rules);
}

void AlertToolController::setMinLevel(int level)
{
  AlertLevel alertLevel = static_cast<AlertLevel>(level);
  switch (alertLevel) {
  case AlertLevel::Low:
  case AlertLevel::Medium:
  case AlertLevel::High:
  case AlertLevel::Critical:
    m_statusAlertRule->setMinStatus(alertLevel);
    m_alertsProxyModel->applyFilter(m_rules);
    break;
  default:
    break;
  }
}

void AlertToolController::flashAll(bool on)
{
  AlertListModel* model = AlertListModel::instance();
  if (!model)
    return;

  const int modelSize = model->rowCount();
  for (int i = 0; i < modelSize; ++i)
  {
    AlertConditionData* alert = model->alertAt(i);
    if (!alert || !alert->active())
      continue;

    alert->highlight(on);
  }
}
