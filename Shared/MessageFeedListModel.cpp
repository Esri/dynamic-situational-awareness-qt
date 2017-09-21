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

#include "MessageFeedListModel.h"
#include "MessageFeed.h"

MessageFeedListModel::MessageFeedListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void MessageFeedListModel::setupRoles()
{
  m_roles[MessageFeedNameRole] = "feedName";
  m_roles[MessageFeedTypeRole] = "feedMessageType";
  m_roles[MessageFeedFormatRole] = "feedMessageFormat";
  m_roles[MessageFeedEnabledRole] = "feedEnabled";
}

bool MessageFeedListModel::isEmpty() const
{
  return rowCount() == 0;
}

void MessageFeedListModel::append(MessageFeed* messageFeed)
{
  if (!messageFeed)
    return;

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_messageFeeds.append(messageFeed);
  endInsertRows();
}

MessageFeed* MessageFeedListModel::at(int index) const
{
  return m_messageFeeds.at(index);
}

void MessageFeedListModel::clear()
{
  beginResetModel();
  m_messageFeeds.clear();
  endResetModel();
}

// QAbstractItemModel interface
int MessageFeedListModel::rowCount(const QModelIndex&) const
{
  return m_messageFeeds.count();
}

QVariant MessageFeedListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_messageFeeds.count())
    return QVariant();

  QVariant retVal;

  MessageFeed* messageFeed = at(index.row());
  if (messageFeed)
  {
    switch (role)
    {
    case MessageFeedNameRole:
      retVal = messageFeed->feedName();
      break;
    case MessageFeedTypeRole:
      retVal.fromValue<Message::MessageType>(messageFeed->feedMessageType());
      break;
    case MessageFeedFormatRole:
      retVal = messageFeed->feedMessageFormat();
      break;
    case MessageFeedEnabledRole:
      retVal = messageFeed->isFeedEnabled();
    default:
      break;
    }
  }

  return retVal;
}

bool MessageFeedListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid())
  {
    MessageFeed* messageFeed = at(index.row());
    if (messageFeed)
    {
      bool isDataChanged = false;

      switch (role)
      {
      case MessageFeedNameRole:
      {
        if (messageFeed->feedName() != value.toString())
        {
          messageFeed->setFeedName(value.toString());

          isDataChanged = true;
        }
        break;
      }
      case MessageFeedTypeRole:
      {
        if (messageFeed->feedMessageType() != value.value<Message::MessageType>())
        {
          messageFeed->setFeedMessageType(value.value<Message::MessageType>());

          isDataChanged = true;
        }
        break;
      }
      case MessageFeedFormatRole:
      {
        if (messageFeed->feedMessageFormat() != value.toString())
        {
          messageFeed->setFeedFormat(value.toString());

          isDataChanged = true;
        }
        break;
      }
      case MessageFeedEnabledRole:
      {
        if (messageFeed->isFeedEnabled() != value.toBool())
        {
          messageFeed->setFeedEnabled(value.toBool());

          isDataChanged = true;
        }
        break;
      }
      default:
        break;
      }

      if (isDataChanged)
        emit dataChanged(index, index, QVector<int>() << role);

      return true;
    }
  }

  return false;
}

QHash<int, QByteArray> MessageFeedListModel::roleNames() const
{
  return m_roles;
}
