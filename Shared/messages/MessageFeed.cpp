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

#include "MessageFeed.h"

// example app headers
#include "MessagesOverlay.h"

namespace Dsa {

/*!
  \class MessageFeed
  \inherits QObject
  \brief Represents a feed for a given message type which will be displayed on a
  \l MessageOverlay.
 */

/*!
  \brief Constructor accepting an optional \a parent.
 */
MessageFeed::MessageFeed(QObject* parent) :
  QObject(parent)
{
}

/*!
  \brief Constructor accepting a feed \a name, a message \a type and an optional \a parent.
 */
MessageFeed::MessageFeed(const QString& name, const QString& type, MessagesOverlay* overlay, QObject* parent) :
  QObject(parent),
  m_feedName(name),
  m_feedMessageType(type),
  m_messagesOverlay(overlay)
{
  updateOverlay();
}

/*!
  \brief Returns the name of the feed.
 */
QString MessageFeed::feedName() const
{
  return m_feedName;
}

/*!
  \brief Sets the name of the feed to \a feedName.
 */
void MessageFeed::setFeedName(const QString& feedName)
{
  m_feedName = feedName;
}

/*!
  \brief Returns the type of this message feed.
 */
QString MessageFeed::feedMessageType() const
{
  return m_feedMessageType;
}

/*!
  \brief Sets the type of this message feed to \a feedMessageType.
 */
void MessageFeed::setFeedMessageType(const QString& feedMessageType)
{
  m_feedMessageType = feedMessageType;
}

/*!
  \brief Returns whether this message feed is visible.
 */
bool MessageFeed::isFeedVisible() const
{
  return m_feedVisible;
}

/*!
  \brief Sets whether this message feed is visible to \a feedVisible.
 */
void MessageFeed::setFeedVisible(bool feedVisible)
{
  if (m_feedVisible == feedVisible)
    return;

  m_feedVisible = feedVisible;

  updateOverlay();
}

/*!
  \brief Returns the \l MessagesOverlay for this feed.
 */
MessagesOverlay* MessageFeed::messagesOverlay() const
{
  return m_messagesOverlay;
}

/*!
  \brief Sets the \l MessagesOverlay for this feed to \a messagesOverlay.
 */
void MessageFeed::setMessagesOverlay(MessagesOverlay* messagesOverlay)
{
  m_messagesOverlay = messagesOverlay;

  updateOverlay();
}

/*!
  \internal
 */
void MessageFeed::updateOverlay()
{
  // update the visibility of the messages overlay
  // that corresponds to this message feed
  if (m_messagesOverlay)
    m_messagesOverlay->setVisible(m_feedVisible);
}

/*!
  \brief Returns the (local file) URL of the thumbnail to use for this feed.
 */
QUrl MessageFeed::thumbnailUrl() const
{
  return m_thumbnailUrl;
}

/*!
  \brief Sets the (local file) URL of the thumbnail to use for this feed to \a thumbnailUrl.
 */
void MessageFeed::setThumbnailUrl(const QUrl& thumbnailUrl)
{
  m_thumbnailUrl = thumbnailUrl;
}

} // Dsa
