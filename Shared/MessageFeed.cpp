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

bool MessageFeed::isFeedEnabled() const
{
  return m_feedEnabled;
}

void MessageFeed::setFeedEnabled(bool feedEnabled)
{
  if (m_feedEnabled == feedEnabled)
    return;

  m_feedEnabled = feedEnabled;

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
    m_messagesOverlay->setVisible(m_feedEnabled);
}
