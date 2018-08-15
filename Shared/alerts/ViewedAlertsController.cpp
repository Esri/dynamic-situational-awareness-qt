
/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

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
  \class Dsa::ViewedAlertsController
  \inmodule Dsa
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
  \property ViewedAlertsController::unviewedCount
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
