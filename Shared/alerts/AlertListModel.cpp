
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

#include "AlertListModel.h"

// example app headers
#include "AlertConditionData.h"

// Qt headers
#include <QUuid>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::AlertListModel
  \inmodule Dsa
  \inherits QAbstractListModel
  \brief A model responsible for storing \l AlertConditionData objects and reporting when they
  change.

  The model returns data for the following roles:
  \table
    \header
        \li Role
        \li Type
        \li Description
    \row
        \li alertId
        \li QUuid
        \li The unique ID of the alert.
    \row
        \li name
        \li QString
        \li The name of the alert.
    \row
        \li level
        \li int
        \li The level of severity.
    \row
        \li viewed
        \li bool
        \li Whether the alert condition has been viewed.
  \endtable
 */

/*!
  \brief Static method to return a singleton instance of the model.
 */
AlertListModel* AlertListModel::instance()
{
  static AlertListModel s_instance;

  return &s_instance;
}

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
AlertListModel::AlertListModel(QObject* parent):
  QAbstractListModel(parent)
{
  // set up the hash of role names
  m_roles[AlertListRoles::AlertId] = "alertId";
  m_roles[AlertListRoles::Name] = "name";
  m_roles[AlertListRoles::Level] = "level";
  m_roles[AlertListRoles::Viewed] = "viewed";
}

/*!
  \brief Destructor.
 */
AlertListModel::~AlertListModel()
{
}

/*!
  \brief Adds a new \l AlertConditionData \a newConditionData to the model.

  Returns \c true on success, else \c false.
 */
bool AlertListModel::addAlertConditionData(AlertConditionData* newConditionData)
{
  if (!newConditionData)
    return false;

  if (!newConditionData->id().isNull())
    return false;

  const int size = m_alerts.size();
  const int insertIdx = size == 0 ? 0 : size;
  const QUuid id = QUuid::createUuid();
  newConditionData->setId(id);

  auto handleDataChanged = [this, newConditionData]()
  {
    if (newConditionData->id().isNull())
      return;

    auto it = m_alerts.cbegin();
    auto itEnd = m_alerts.cend();
    int currRow = 0;
    for (; it != itEnd; ++it, ++currRow)
    {
      AlertConditionData* testAlert = *it;
      if (!testAlert)
        continue;

      if (testAlert->id() == newConditionData->id())
      {
        const QModelIndex changedIndex = index(currRow, 0);
        emit dataChanged(changedIndex, changedIndex);
      }
    }
  };

  connect(newConditionData, &AlertConditionData::viewedChanged, this, handleDataChanged);
  connect(newConditionData, &AlertConditionData::dataChanged, this, handleDataChanged);

  connect(newConditionData, &AlertConditionData::noLongerValid, this, [this, newConditionData]
  {
    removeAlert(newConditionData);
  });

  beginInsertRows(QModelIndex(), insertIdx, insertIdx);
  m_alerts.append(newConditionData);
  endInsertRows();

  return true;
}

/*!
  \brief Removes \a conditionData from the model.
 */
void AlertListModel::removeAlert(AlertConditionData* conditionData)
{
  if (!conditionData)
    return;

  if (conditionData->id().isNull())
    return;

  const QUuid theId = conditionData->id();

  for (int i = 0; i < m_alerts.size(); ++i)
  {
    AlertConditionData* testAlert = m_alerts.at(i);
    if (!testAlert)
      continue;

    if (testAlert->id() != theId)
      continue;

    removeAt(i);
    break;
  }
}

/*!
  \brief Return the \l AlertConditionData at \a rowIndex.
 */
AlertConditionData* AlertListModel::alertAt(int rowIndex) const
{
  return m_alerts.value(rowIndex, nullptr);
}

/*!
  \brief Removes the \l AlertConditionData at \a rowIndex.
 */
void AlertListModel::removeAt(int rowIndex)
{
  AlertConditionData* alert = alertAt(rowIndex);
  if (!alert)
    return;

  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
  m_alerts.removeAt(rowIndex);
  endRemoveRows();
}

/*!
  \brief Returns the number of condition data objects in the model.
 */
int AlertListModel::rowCount(const QModelIndex&) const
{
  return m_alerts.size();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l AlertListRoles enum.
 */
QVariant AlertListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() > rowCount())
    return QVariant();

  AlertConditionData* alert = m_alerts.at(index.row());
  if (!alert)
    return QVariant();

  switch (role)
  {
  case AlertListRoles::AlertId:
    return alert->id();
  case AlertListRoles::Level:
    return static_cast<int>(alert->level());
  case AlertListRoles::Name:
  {
    return alert->name();
  }
  case AlertListRoles::Viewed:
  {
    return alert->viewed();
  }
  default:
    break;
  }

  return QVariant();
}

/*!
  \brief Sets the data stored under \a role at \a index in the model to \a value.

  The role should make use of the \l AlertListRoles enum.

  Return \c true if the data was successfully set and \c false otherwise.
 */
bool AlertListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || value.isNull())
    return false;

  AlertConditionData* alert = alertAt(index.row());
  if (!alert)
    return false;

  bool valueSet = false;
  switch (role)
  {
  case AlertListRoles::AlertId:
    break;
  case AlertListRoles::Level:
    break;
  case AlertListRoles::Name:
    break;
  case AlertListRoles::Viewed:
  {
    const bool newViewed = value.toBool();
    if (alert->viewed() != newViewed)
    {
      alert->setViewed(newViewed);
      valueSet = true;
    }
    break;
  }
  default:
    break;
  }

  if (valueSet)
    dataChanged(index, index);

  return valueSet;
}

/*!
  \brief Returns the hash of role names used by the model.

  The roles are based on the \l AlertListRoles enum.
 */
QHash<int, QByteArray> AlertListModel::roleNames() const
{
  return m_roles;
}

} // Dsa
