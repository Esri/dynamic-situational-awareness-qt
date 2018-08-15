
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
