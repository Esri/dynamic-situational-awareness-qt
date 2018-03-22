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

// PCH header
#include "pch.hpp"

#include "AlertListController.h"

// example app headers
#include "AlertConditionData.h"
#include "AlertListModel.h"
#include "AlertListProxyModel.h"
#include "AlertSource.h"
#include "DsaUtility.h"
#include "IdsAlertFilter.h"
#include "PointHighlighter.h"
#include "StatusAlertFilter.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "GeoView.h"
#include "GraphicsOverlay.h"
#include "SceneView.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleRenderer.h"

// Qt headers
#include <QTimer>

using namespace Esri::ArcGISRuntime;

/*!
  \class AlertListController
  \inherits Toolkit::AbstractTool
  \brief Tool controller for working with the list of condition data which can trigger alerts.

  Alerts are created when a given \l AlertCondition is met.

  This tool manages the list of current condition data and presents a filtered list of those
  which are active.

  It also allows individual alerts to highlighted, zoomed to and marked as viewed.

  \sa AlertListModel
  \sa AlertListProxyModel
  \sa AlertConditionData
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
AlertListController::AlertListController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_alertsProxyModel(new AlertListProxyModel(AlertListModel::instance(), this)),
  m_statusAlertFilter(new StatusAlertFilter(this)),
  m_idsAlertFilter(new IdsAlertFilter(this)),
  m_highlighter(new PointHighlighter(this))
{
  Toolkit::ToolManager::instance().addTool(this);
  m_filters.append(m_statusAlertFilter);
  m_filters.append(m_idsAlertFilter);

  // sets the initial set of filters for condition data
  m_alertsProxyModel->applyFilter(m_filters);

  connect(AlertListModel::instance(), &AlertListModel::dataChanged, this, &AlertListController::allAlertsCountChanged);
  connect(AlertListModel::instance(), &AlertListModel::rowsInserted, this, &AlertListController::allAlertsCountChanged);
  connect(AlertListModel::instance(), &AlertListModel::rowsRemoved, this, &AlertListController::allAlertsCountChanged);
  emit allAlertsCountChanged();
}

/*!
  \brief Destructor.
 */
AlertListController::~AlertListController()
{
}

/*!
  \brief Returns a model containing the filtered list of active alert condition data.
 */
QAbstractItemModel* AlertListController::alertListModel() const
{
  return m_alertsProxyModel;
}

/*!
  \brief Returns the count of all alerts - ignoring any filters.
 */
int AlertListController::allAlertsCount() const
{
  AlertListModel* model = AlertListModel::instance();
  if (!model)
    return 0;

  int count = 0;
  const int alertsCount = model->rowCount();
  for (int i = 0; i < alertsCount; ++i)
  {
    AlertConditionData* alert = model->alertAt(i);
    if (!alert)
      continue;

    if (!alert->isActive())
      continue;

    if (!alert->isConditionEnabled())
      continue;

    count++;
  }

  return count;
}

/*!
  \brief Returns the name of this tool.
 */
QString AlertListController::toolName() const
{
  return "Alert List";
}

/*!
  \brief Sets the highlight state for the active condition data at \a rowIndex in the filtered model to \a showHighlight.

  \note Only one alert can be highlighted at a given time.

  \sa PointHighlighter.
 */
void AlertListController::highlight(int rowIndex, bool showHighlight)
{
  if (showHighlight)
  {
    QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
    AlertConditionData* conditionData = AlertListModel::instance()->alertAt(sourceIndex.row());

    if (!conditionData)
      return;

    for (const auto& connection : m_highlightConnections)
      disconnect(connection);

    m_highlightConnections.clear();

    m_highlightConnections.append(connect(conditionData, &AlertConditionData::noLongerValid, this, [this]()
    {
      m_highlighter->stopHighlight();
    }));

    m_highlightConnections.append(connect(conditionData->source(), &AlertSource::dataChanged, this, [this, conditionData]()
    {
      if (!conditionData)
        return;

      m_highlighter->onPointChanged(conditionData->sourceLocation());

      if (!conditionData->isActive())
        m_highlighter->stopHighlight();
    }));

    for (AlertFilter* filter : m_filters)
    {
      // when a filter changes, check whether it should exclude the alert: if so, stop the highlight
      m_highlightConnections.append(connect(filter, &AlertFilter::filterChanged, this, [this, conditionData, filter]()
      {
        if (!conditionData)
          return;

        if (!filter->passesFilter(conditionData))
        {
          m_highlighter->stopHighlight();
          emit highlightStopped();
        }
      }));
    }

    m_highlighter->onPointChanged(conditionData->sourceLocation());
    m_highlighter->startHighlight();
  }
  else
  {
    for (const auto& connection : m_highlightConnections)
      disconnect(connection);

    m_highlightConnections.clear();

    m_highlighter->stopHighlight();
    emit highlightStopped();
  }
}

/*!
  \brief Zooms the geo view to the active alert condition data at \a rowIndex in the filtered model.
 */
void AlertListController::zoomTo(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AlertConditionData* alert = AlertListModel::instance()->alertAt(sourceIndex.row());
  if (!alert)
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  const Point pos = alert->sourceLocation().extent().center();

  SceneView* sceneView = dynamic_cast<SceneView*>(geoView);

  if (sceneView)
  {
    const Camera currentCam = sceneView->currentViewpointCamera();
    constexpr double targetDistance = 1500.0;
    Camera newCam(pos, targetDistance, currentCam.heading(), currentCam.pitch(), currentCam.roll());

    sceneView->setViewpointCamera(newCam, 1.0);
  }
  else
  {
    const Viewpoint currVP = geoView->currentViewpoint(ViewpointType::CenterAndScale);
    const Viewpoint newViewPoint(pos, currVP.targetScale());

    geoView->setViewpoint(newViewPoint, 1.0);
  }
}

/*!
  \brief Sets the viewed state of the active alert at index \a rowIndex in the filtered model to \c true.
 */
void AlertListController::setViewed(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AlertListModel* model = AlertListModel::instance();
  if (!model)
    return;

  model->setData(sourceIndex, QVariant::fromValue(true), AlertListModel::AlertListRoles::Viewed);
}

/*!
  \brief Dismiss the active alert at index \a rowIndex in the filtered model.

  This will add the ID of the alert condition data to the current \l IdsAlertFilter.
 */
void AlertListController::dismiss(int rowIndex)
{
  QModelIndex sourceIndex = m_alertsProxyModel->mapToSource(m_alertsProxyModel->index(rowIndex, 0));
  AlertConditionData* alert = AlertListModel::instance()->alertAt(sourceIndex.row());
  if (!alert)
    return;

  m_idsAlertFilter->addId(alert->id());
  m_alertsProxyModel->applyFilter(m_filters);
}


/*!
  \brief Sets the minimum \l AlertLevel for the current \l StatusAlertFilter to \a level.
 */
void AlertListController::setMinLevel(int level)
{
  AlertLevel alertLevel = static_cast<AlertLevel>(level);
  switch (alertLevel) {
  case AlertLevel::Low:
  case AlertLevel::Medium:
  case AlertLevel::High:
  case AlertLevel::Critical:
    m_statusAlertFilter->setMinLevel(alertLevel);
    m_alertsProxyModel->applyFilter(m_filters);
    break;
  default:
    break;
  }
}

/*!
  \brief Clears any filters applied to the alerts.
 */
void AlertListController::clearAllFilters()
{
  m_idsAlertFilter->clearIds();
  m_statusAlertFilter->setMinLevel(AlertLevel::Low);
  m_alertsProxyModel->applyFilter(m_filters);
}

/*!
  \brief Sets the highlight state for all currently active condition data to \a highlight.
 */
void AlertListController::flashAll(bool highlight)
{
  AlertListModel* model = AlertListModel::instance();
  if (!model)
    return;

  const int modelSize = model->rowCount();
  for (int i = 0; i < modelSize; ++i)
  {
    AlertConditionData* alert = model->alertAt(i);
    if (!alert || !alert->isActive() || !alert->isConditionEnabled())
      continue;

    alert->highlight(highlight);
  }
}
