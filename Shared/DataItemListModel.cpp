// Copyright 2016 ESRI
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

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include "DataItemListModel.h"
#include "DataItem.h"

DataItemListModel::DataItemListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void DataItemListModel::setupRoles()
{
  m_roles[FullPathRole] = "fullPath";
  m_roles[FileNameRole] = "fileName";
  m_roles[DataTypeRole] = "dataType";
}
void DataItemListModel::addDataItem(DataItem* dataItem)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_dataItems << dataItem;
  endInsertRows();
  emit sizeChanged();
}
int DataItemListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_dataItems.count();
}

QVariant DataItemListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_dataItems.count())
    return QVariant();

  DataItem* dataItem = m_dataItems[index.row()];
  QVariant retVal;  

  switch (role)
  {
  case FullPathRole:
    retVal = dataItem->fullPath();
    break;
  case FileNameRole:
    retVal = dataItem->fileName();
    break;
  case DataTypeRole:
    retVal = dataItem->dataTypeAsString();
    break;
  default:
    break;
  }

  return retVal;
}

QHash<int, QByteArray> DataItemListModel::roleNames() const
{
  return m_roles;
}

void DataItemListModel::clear()
{
  for (DataItem* dataItem : m_dataItems)
  {
    dataItem->deleteLater();
  }
  beginResetModel();
  m_dataItems.clear();
  endResetModel();
}
