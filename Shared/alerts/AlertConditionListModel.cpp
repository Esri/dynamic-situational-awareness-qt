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

#include "AlertConditionListModel.h"

// example app headers
#include "AlertCondition.h"

namespace Dsa {

/*!
  \class AlertConditionListModel
  \inherits QAbstractListModel
  \brief A model for storing \l AlertCondition objects and reporting when they
  change.
  */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
AlertConditionListModel::AlertConditionListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[AlertConditionListRoles::ConditionName] = "conditionName";
  m_roles[AlertConditionListRoles::Level] = "level";
  m_roles[AlertConditionListRoles::Description] = "description";
  m_roles[AlertConditionListRoles::ConditionEnabled] = "conditionEnabled";
}

/*!
  \brief Destructor.
 */
AlertConditionListModel::~AlertConditionListModel()
{

}

/*!
  \brief Adds a new \l AlertCondition \a condition to the model.
 */
bool AlertConditionListModel::addAlertCondition(AlertCondition* condition)
{
  if (!condition)
    return false;

  const int size = m_conditions.size();
  const int insertIdx = size == 0 ? 0 : size;

  connect(condition, &AlertCondition::conditionChanged, this, [this, condition]()
  {
    auto it = m_conditions.cbegin();
    auto itEnd = m_conditions.cend();
    int currRow = 0;
    for (; it != itEnd; ++it, ++currRow)
    {
      AlertCondition* candidateCondition = *it;
      if (!candidateCondition)
        continue;

      if (candidateCondition == condition)
      {
        const QModelIndex changedIndex = index(currRow, 0);
        emit dataChanged(changedIndex, changedIndex);
      }
    }
  });

  beginInsertRows(QModelIndex(), insertIdx, insertIdx);
  m_conditions.append(condition);
  endInsertRows();

  return true;
}

/*!
  \brief Return the \l AlertCondition at \a rowIndex.
 */
AlertCondition* AlertConditionListModel::conditionAt(int rowIndex) const
{
  return m_conditions.value(rowIndex, nullptr);
}

/*!
  \brief Removes condition at \a rowIndex from the model.
 */
void AlertConditionListModel::removeAt(int rowIndex)
{
  AlertCondition* condition = conditionAt(rowIndex);
  if (!condition)
    return;

  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
  m_conditions.removeAt(rowIndex);
  endRemoveRows();
}

/*!
  \brief Returns the number of condition objects in the model.
 */
int AlertConditionListModel::rowCount(const QModelIndex&) const
{
  return m_conditions.size();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l AlertConditionListRoles enum.
 */
QVariant AlertConditionListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() > rowCount())
    return QVariant();

  AlertCondition* condition = m_conditions.at(index.row());
  if (!condition)
    return QVariant();

  switch (role)
  {
  case AlertConditionListRoles::ConditionName:
    return condition->name();
  case AlertConditionListRoles::Level:
    return static_cast<int>(condition->level());
  case AlertConditionListRoles::Description:
    return condition->description();
  case AlertConditionListRoles::ConditionEnabled:
    return condition->isConditionEnabled();
  default:
    break;
  }

  return QVariant();
}

/*!
  \brief Sets the data stored under \a role at \a index in the model to \a value.

  The role should make use of the \l AlertConditionListRoles enum.

  Return \c true if the data was successfully set and \c false otherwise.
 */
bool AlertConditionListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || value.isNull())
    return false;

  AlertCondition* condition = conditionAt(index.row());
  if (!condition)
    return false;

  bool valueSet = false;
  switch (role)
  {
  case AlertConditionListRoles::ConditionName:
  {
    condition->setName(value.toString());
    valueSet = true;
  }
  case AlertConditionListRoles::Level:
  {
    bool ok = false;
    AlertLevel level = static_cast<AlertLevel>(value.toInt(&ok));

    if (level != AlertLevel::Unknown && ok)
    {
      condition->setLevel(level);
      valueSet = true;
    }
  }
  case AlertConditionListRoles::Description:
    break;
  case AlertConditionListRoles::ConditionEnabled:
  {
    const bool newEnabled = value.toBool();
    condition->setConditionEnabled(newEnabled);
    valueSet = true;
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
QHash<int, QByteArray> AlertConditionListModel::roleNames() const
{
  return m_roles;
}

} // Dsa
