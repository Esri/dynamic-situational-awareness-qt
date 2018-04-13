// Copyright 2018 ESRI
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

#include "ViewedAlertsController.h"

// example app headers
#include "AlertConditionData.h"
#include "AlertListModel.h"

// toolkit headers
#include "ToolManager.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class ViewedAlertsController
  \inherits Toolkit::AbstractTool
  \brief Tool controller for managing the count of unviewed, active alert condition data.

  Alerts are created when a given \l AlertCondition is met.

  This tool reports changes to the total number of alert condition data which are
  active but have not been marked as viwed.

  \sa AlertListModel
  \sa AlertConditionData
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
ViewedAlertsController::ViewedAlertsController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  AlertListModel* model = AlertListModel::instance();
  if (model)
  {
    connect(model, &AlertListModel::dataChanged, this, &ViewedAlertsController::handleDataChanged);
    connect(model, &AlertListModel::rowsInserted, this, &ViewedAlertsController::handleDataChanged);
    connect(model, &AlertListModel::rowsRemoved, this, &ViewedAlertsController::handleDataChanged);
    emit unviewedCountChanged();
  }
}

/*!
  \brief The name of this tool.
 */
QString ViewedAlertsController::toolName() const
{
  return QString("viewed alerts");
}

void ViewedAlertsController::handleDataChanged()
{
  const int oldCount = m_cachedCount;
  m_cachedCount = -1;
  m_cachedCount = unviewedCount();

  if (oldCount != m_cachedCount)
    emit unviewedCountChanged();
}

/*!
  \brief Destructor.
 */
ViewedAlertsController::~ViewedAlertsController()
{
}

/*!
  \brief Returns the number of alert condition data objects which are currently active
  and which have not been marked as viewed.
 */
int ViewedAlertsController::unviewedCount() const
{
  if (m_cachedCount != -1)
    return m_cachedCount;

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

    if (alert->viewed())
      continue;

    count++;
  }

  m_cachedCount = count;

  return m_cachedCount;
}


} // Dsa

// Signal Documentation
/*!
  \fn void ViewedAlertsController::unviewedCountChanged();
  \brief Signal emitted when the unviewed count changes.
 */
