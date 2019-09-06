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

#include "MessageFeedListModel.h"

// dsa app headers
#include "MessageFeed.h"

namespace Dsa {

/*!
  \class Dsa::MessageFeedListModel
  \inmodule Dsa
  \inherits QAbstractListModel
  \brief A model responsible for storing \l MessageFeed objects and reporting when they
  change.

  The model returns data for the following roles:
  \table
    \header
        \li Role
        \li Type
        \li Description
    \row
        \li feedName
        \li QString
        \li The name of the message feed.
    \row
        \li feedMessageType
        \li QString
        \li The type of message.
    \row
        \li feedVisible
        \li bool
        \li Whether the feed is visible.
    \row
        \li thumbnailUrl
        \li QUrl
        \li The URL to the Message's thumbnail.
  \endtable
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
MessageFeedListModel::MessageFeedListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

/*!
  \internal
 */
void MessageFeedListModel::setupRoles()
{
  m_roles[MessageFeedNameRole] = "feedName";
  m_roles[MessageFeedTypeRole] = "feedMessageType";
  m_roles[MessageFeedVisibleRole] = "feedVisible";
  m_roles[MessageFeedThumbnailUrlRole] = "thumbnailUrl";
}

/*!
  \brief Returns whether the model is empty.
 */
bool MessageFeedListModel::isEmpty() const
{
  return rowCount() == 0;
}

/*!
  \brief Adds a new \l MessageFeed \a messageFeed to the model.
 */
void MessageFeedListModel::append(MessageFeed* messageFeed)
{
  if (!messageFeed)
    return;

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_messageTypes.append(messageFeed->feedMessageType());
  m_messageFeeds.append(messageFeed);
  endInsertRows();
}

/*!
  \brief Return the \l MessageFeed at \a index.
 */
MessageFeed* MessageFeedListModel::at(int index) const
{
  if (index < 0 || (index + 1) > rowCount())
    return nullptr;

  return m_messageFeeds.at(index);
}

/*!
  \brief Returns a \l MessageFeed of the supplied \a type if one is found.

  If no feed of the supplied type is found, returns \c nullptr.
 */
MessageFeed* MessageFeedListModel::messageFeedByType(const QString& type) const
{
  int index = m_messageTypes.indexOf(type);
  if (index == -1)
    return nullptr;

  return m_messageFeeds[index];
}

/*!
  \brief Clears the model.
 */
void MessageFeedListModel::clear()
{
  beginResetModel();
  m_messageTypes.clear();
  m_messageFeeds.clear();
  endResetModel();
}

/*!
  \brief Returns the number of message feed objects in the model.
 */
int MessageFeedListModel::rowCount(const QModelIndex&) const
{
  return m_messageFeeds.count();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l MessageFeedRoles enum.
 */
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

/*!
  \brief Sets the data stored under \a role at \a index in the model to \a value.

  The role should make use of the \l MessageFeedRoles enum.

  Return \c true if the data was successfully set and \c false otherwise.
 */
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

/*!
  \brief Returns the hash of role names used by the model.

  The roles are based on the \l AlertListRoles enum.
 */
QHash<int, QByteArray> MessageFeedListModel::roleNames() const
{
  return m_roles;
}

/*!
  \brief Returns the number of rows in the model.
 */
int MessageFeedListModel::count() const
{
  return m_messageFeeds.length();
}

} // Dsa
