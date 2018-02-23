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

#include "MessageFeed.h"
#include "MessagesOverlay.h"
#include "MessageListener.h"

MessageFeed::MessageFeed(QObject* parent) :
  QObject(parent)
{
}

MessageFeed::MessageFeed(const QString& name, const QString& type, MessagesOverlay* overlay, QObject* parent) :
  QObject(parent),
  m_feedName(name),
  m_feedMessageType(type),
  m_messagesOverlay(overlay)
{
  updateOverlay();
}

QString MessageFeed::feedName() const
{
  return m_feedName;
}

void MessageFeed::setFeedName(const QString& feedName)
{
  m_feedName = feedName;
}

QString MessageFeed::feedMessageType() const
{
  return m_feedMessageType;
}

void MessageFeed::setFeedMessageType(const QString& feedMessageType)
{
  m_feedMessageType = feedMessageType;
}

bool MessageFeed::isFeedVisible() const
{
  return m_feedVisible;
}

void MessageFeed::setFeedVisible(bool feedVisible)
{
  if (m_feedVisible == feedVisible)
    return;

  m_feedVisible = feedVisible;

  updateOverlay();
}

MessagesOverlay* MessageFeed::messagesOverlay() const
{
  return m_messagesOverlay;
}

void MessageFeed::setMessagesOverlay(MessagesOverlay* messagesOverlay)
{
  m_messagesOverlay = messagesOverlay;

  updateOverlay();
}

void MessageFeed::updateOverlay()
{
  // update the visibility of the messages overlay
  // that corresponds to this message feed
  if (m_messagesOverlay)
    m_messagesOverlay->setVisible(m_feedVisible);
}

QUrl MessageFeed::iconUrl() const
{
    return m_iconUrl;
}

void MessageFeed::setIconUrl(const QUrl& iconUrl)
{
    m_iconUrl = iconUrl;
}
