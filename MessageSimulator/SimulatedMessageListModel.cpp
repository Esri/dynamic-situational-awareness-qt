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

#include "SimulatedMessageListModel.h"
#include "SimulatedMessage.h"

SimulatedMessageListModel::SimulatedMessageListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

SimulatedMessageListModel::~SimulatedMessageListModel()
{
}

void SimulatedMessageListModel::setupRoles()
{
  m_roles[FormatRole] = "messageFormat";
  m_roles[IdRole] = "messageId";
  m_roles[ActionRole] = "messageAction";
  m_roles[SymbolIdRole] = "symbolId";
}

void SimulatedMessageListModel::append(SimulatedMessage* message)
{
  if (!message)
    return;

  beginInsertRows(QModelIndex(), rowCount(), rowCount());

  m_messages.append(message);

  endInsertRows();
}

void SimulatedMessageListModel::clear()
{
  if (rowCount() > 0)
  {
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);

    qDeleteAll(m_messages);
    m_messages.clear();

    endRemoveRows();
  }
}

Qt::ItemFlags SimulatedMessageListModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
      return Qt::ItemIsEnabled;

  return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

int SimulatedMessageListModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return m_messages.size();
}

QVariant SimulatedMessageListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount())
    return QVariant();

  QVariant retVal;

  SimulatedMessage* message = m_messages.at(index.row());
  if (message)
  {
    switch (role)
    {
    case FormatRole:
      retVal = message->messageFormatString();
      break;
    case IdRole:
      retVal = message->messageId();
      break;
    case ActionRole:
      retVal = message->messageAction();
      break;
    case SymbolIdRole:
      retVal = message->symbolId();
      break;
    default:
      break;
    }
  }

  return retVal;
}

bool SimulatedMessageListModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
    return false;

  beginRemoveRows(QModelIndex(), row, row + count - 1);

  for (int r = row; r < row + count; ++r)
  {
    SimulatedMessage* message = m_messages.at(r);
    m_messages.removeAt(r);
    if (message)
      delete message;
  }

  endRemoveRows();

  return true;
}

QHash<int, QByteArray> SimulatedMessageListModel::roleNames() const
{
  return m_roles;
}
