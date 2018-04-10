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

// PCH header
#include "pch.hpp"

#include "MessageFeedListModel.h"

// example app headers
#include "MessageFeed.h"

namespace Dsa {

MessageFeedListModel::MessageFeedListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void MessageFeedListModel::setupRoles()
{
  m_roles[MessageFeedNameRole] = "feedName";
  m_roles[MessageFeedTypeRole] = "feedMessageType";
  m_roles[MessageFeedVisibleRole] = "feedVisible";
  m_roles[MessageFeedThumbnailUrlRole] = "thumbnailUrl";
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
  m_messageTypes.append(messageFeed->feedMessageType());
  m_messageFeeds.append(messageFeed);
  endInsertRows();
}

MessageFeed* MessageFeedListModel::at(int index) const
{
  if (index < 0 || (index + 1) > rowCount())
    return nullptr;

  return m_messageFeeds.at(index);
}

MessageFeed* MessageFeedListModel::messageFeedByType(const QString& type) const
{
  int index = m_messageTypes.indexOf(type);
  if (index == -1)
    return nullptr;

  return m_messageFeeds[index];
}

void MessageFeedListModel::clear()
{
  beginResetModel();
  m_messageTypes.clear();
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
  if (!messageFeed)
    return retVal;

  switch (role)
  {
  case MessageFeedNameRole:
    retVal = messageFeed->feedName();
    break;
  case MessageFeedTypeRole:
    retVal = messageFeed->feedMessageType();
    break;
  case MessageFeedVisibleRole:
    retVal = messageFeed->isFeedVisible();
    break;
  case MessageFeedThumbnailUrlRole:
    retVal = messageFeed->thumbnailUrl();
    break;
  default:
    break;
  }

  return retVal;
}

bool MessageFeedListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
    return false;

  MessageFeed* messageFeed = at(index.row());
  if (!messageFeed)
    return false;

  bool isDataChanged = false;

  switch (role)
  {
  case MessageFeedNameRole:
  {
    const auto val = value.toString();
    if (messageFeed->feedName() != val)
    {
      messageFeed->setFeedName(val);

      isDataChanged = true;
    }
    break;
  }
  case MessageFeedTypeRole:
  {
    const auto val = value.toString();
    if (messageFeed->feedMessageType() != val)
    {
      messageFeed->setFeedMessageType(val);

      isDataChanged = true;
    }
    break;
  }
  case MessageFeedVisibleRole:
  {
    const auto val = value.toBool();
    if (messageFeed->isFeedVisible() != val)
    {
      messageFeed->setFeedVisible(val);

      isDataChanged = true;
    }
    break;
  }
  default:
    break;
  }

  if (isDataChanged)
    emit dataChanged(index, index, QVector<int>{role});

  return true;
}

QHash<int, QByteArray> MessageFeedListModel::roleNames() const
{
  return m_roles;
}

int MessageFeedListModel::count() const
{
  return m_messageFeeds.length();
}

} // Dsa
