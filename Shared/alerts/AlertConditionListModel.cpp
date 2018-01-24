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

#include "AlertCondition.h"
#include "AlertConditionListModel.h"

AlertConditionListModel::AlertConditionListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[AlertConditionListRoles::Name] = "name";
  m_roles[AlertConditionListRoles::Level] = "level";
}

AlertConditionListModel::~AlertConditionListModel()
{

}

bool AlertConditionListModel::addAlertCondition(AlertCondition* condition)
{
  if (!condition)
    return false;

  const int size = m_conditions.size();
  const int insertIdx = size == 0 ? 0 : size -1;

  beginInsertRows(QModelIndex(), insertIdx, insertIdx);
  m_conditions.append(condition);
  endInsertRows();

  return true;
}

AlertCondition* AlertConditionListModel::conditionAt(int rowIndex) const
{
  return m_conditions.value(rowIndex, nullptr);
}

void AlertConditionListModel::removeAt(int rowIndex)
{
  AlertCondition* condition = conditionAt(rowIndex);
  if (!condition)
    return;

  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
  m_conditions.removeAt(rowIndex);
  endRemoveRows();
}

int AlertConditionListModel::rowCount(const QModelIndex&) const
{
  return m_conditions.size();
}

QVariant AlertConditionListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() > rowCount())
    return QVariant();

  AlertCondition* condition = m_conditions.at(index.row());
  if (!condition)
    return QVariant();

  switch (role)
  {
  case AlertConditionListRoles::Name:
    return condition->name();
  case AlertConditionListRoles::Level:
    return static_cast<int>(condition->level());
  default:
    break;
  }

  return QVariant();
}

QHash<int, QByteArray> AlertConditionListModel::roleNames() const
{
  return m_roles;
}
