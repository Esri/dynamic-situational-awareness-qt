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

#include "MessageFeed.h"
#include "MessagesOverlay.h"
#include "MessageListener.h"

MessageFeed::MessageFeed(QObject* parent) :
  QObject(parent)
{
}

MessageFeed::MessageFeed(const QString& name, Message::MessageType type, const QString& format, MessagesOverlay* overlay, MessageListener* listener, QObject* parent) :
  QObject(parent),
  m_feedName(name),
  m_feedMessageType(type),
  m_feedMessageFormat(format),
  m_messagesOverlay(overlay),
  m_messageListener(listener)
{
  updateOverlayandListener();
}

QString MessageFeed::feedName() const
{
  return m_feedName;
}

void MessageFeed::setFeedName(const QString& feedName)
{
  m_feedName = feedName;
}

Message::MessageType MessageFeed::feedMessageType() const
{
  return m_feedMessageType;
}

void MessageFeed::setFeedMessageType(Message::MessageType feedMessageType)
{
  m_feedMessageType = feedMessageType;
}

QString MessageFeed::feedMessageFormat() const
{
  return m_feedMessageFormat;
}

void MessageFeed::setFeedMessageFormat(const QString& feedMessageFormat)
{
  m_feedMessageFormat = feedMessageFormat;
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

  updateOverlayandListener();
}

MessagesOverlay* MessageFeed::messagesOverlay() const
{
  return m_messagesOverlay;
}

void MessageFeed::setMessagesOverlay(MessagesOverlay* messagesOverlay)
{
  m_messagesOverlay = messagesOverlay;

  updateOverlayandListener();
}

MessageListener* MessageFeed::messageListener() const
{
  return m_messageListener;
}

void MessageFeed::setMessageListener(MessageListener* messageListener)
{
  m_messageListener = messageListener;

  updateOverlayandListener();
}

void MessageFeed::updateOverlayandListener()
{
  // update the visibility of the messages overlay
  // that corresponds to this message feed
  if (m_messagesOverlay)
    m_messagesOverlay->setVisible(m_feedEnabled);

  // enable or disable the message listener appropriately
  if (m_messageListener)
    m_messageListener->setEnabled(m_feedEnabled);
}
