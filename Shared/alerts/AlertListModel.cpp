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

#include "pch.hpp"

#include "AlertConditionData.h"
#include "AlertListModel.h"

#include <QUuid>

using namespace Esri::ArcGISRuntime;

/*!
  \class AlertListModel
  \inherits QAbstractListModel
  \brief A model responsible for storing \l AlertConditionData objects and reporting when they
  change.
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
