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
#include "IntersectsAlertRule.h"
#include "ProximityAlertRule.h"
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
  m_distanceAlertRule(new ProximityAlertRule(this)),
  m_intersectsRule(new IntersectsAlertRule(this)),
  m_statusAlertRule(new StatusAlertRule(this)),
  m_idsAlertRule(new IdsAlertRule(this))
{
  Toolkit::ToolManager::instance().addTool(this);
  m_rules.append(m_distanceAlertRule);
  m_rules.append(m_intersectsRule);
  m_rules.append(m_statusAlertRule);
  m_rules.append(m_idsAlertRule);

  m_alertsProxyModel->applyFilter(m_rules);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged,
          this, &AlertToolController::onGeoviewChanged);

  onGeoviewChanged();
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

void AlertToolController::highlight(int rowIndex, bool on)
{
  if (on)
  {
    QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
    AbstractAlert* alert = AlertListModel::instance()->alertAt(sourceIndex.row());

    if (!alert)
      return;

    if (!m_highlightOverlay || !m_highlightSymbol)
      return;

    const Geometry alertPosition = alert->position();

    Graphic* highlightGraphic = new Graphic(alertPosition, m_highlightSymbol, this);
    m_highlightOverlay->graphics()->append(highlightGraphic);

    m_highlightTimer = new QTimer(this);
    connect(m_highlightTimer, &QTimer::timeout, this, [this, alert]()
    {
      if (!m_highlightSymbol || !alert)
      {
        m_highlightTimer->stop();
        return;
      }

      Graphic* graphic = m_highlightOverlay->graphics()->first();
      if (!graphic)
      {
        m_highlightTimer->stop();
        return;
      }

      graphic->setGeometry(alert->position());

      const int currDim = m_highlightSymbol->width();
      constexpr int maxDimension = 1000;
      int newDimension = currDim;
      float newOpacity = m_highlightOverlay->opacity();
      if (currDim < maxDimension)
      {
        newDimension += 10;
        newOpacity -= 0.01f;
      }
      else
      {
        newDimension = 1;
      }

      if (currDim > 10 && currDim < 30)
        newOpacity = 1.0f;

      m_highlightSymbol->setWidth(newDimension);
      m_highlightSymbol->setHeight(newDimension);
      m_highlightSymbol->setDepth(newDimension);
      m_highlightOverlay->setOpacity(newOpacity);
    });

    m_highlightTimer->start(10);
  }
  else
  {
    Graphic* graphic = m_highlightOverlay->graphics()->first();
    if (graphic)
    {
      delete graphic;
      m_highlightOverlay->graphics()->clear();
    }

    if (m_highlightTimer)
    {
      m_highlightTimer->stop();
      delete m_highlightTimer;
      m_highlightTimer = nullptr;
    }
  }

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

void AlertToolController::flashAll(bool on)
{
  AlertListModel* model = AlertListModel::instance();
  if (!model)
    return;

  const int modelSize = model->rowCount();
  for (int i = 0; i < modelSize; ++i)
  {
    AbstractAlert* alert = model->alertAt(i);
    if (!alert || !alert->active())
      continue;

    alert->highlight(on);
  }
}

void AlertToolController::onGeoviewChanged()
{
  if (m_highlightOverlay)
  {
    delete m_highlightOverlay;
    m_highlightOverlay = nullptr;
  }

  if (m_highlightSymbol)
  {
    delete m_highlightSymbol;
    m_highlightSymbol = nullptr;
  }

  GeoView* geoview = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoview)
    return;

  m_highlightOverlay = new GraphicsOverlay(this);
  geoview->graphicsOverlays()->append(m_highlightOverlay);

  m_highlightSymbol = new SimpleMarkerSceneSymbol(
        SimpleMarkerSceneSymbolStyle::Sphere, Qt::red, 1.0, 1.0, 1.0, SceneSymbolAnchorPosition::Center, this);
}
