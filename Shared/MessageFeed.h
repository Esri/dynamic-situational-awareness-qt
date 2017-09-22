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

#ifndef MESSAGEFEED_H
#define MESSAGEFEED_H

#include <QObject>

#include "Message.h"

class MessagesOverlay;
class MessageListener;

class MessageFeed : public QObject
{
  Q_OBJECT

public:
  explicit MessageFeed(QObject* parent = nullptr);
  MessageFeed(const QString& name, Message::MessageType type, const QString& format, MessagesOverlay* overlay, MessageListener* listener, QObject* parent = nullptr);
  ~MessageFeed() = default;

  QString feedName() const;
  void setFeedName(const QString& feedName);

  Message::MessageType feedMessageType() const;
  void setFeedMessageType(Message::MessageType feedMessageType);

  QString feedMessageFormat() const;
  void setFeedMessageFormat(const QString& feedMessageFormat);

  bool isFeedEnabled() const;
  void setFeedEnabled(bool feedEnabled);

  MessagesOverlay* messagesOverlay() const;
  void setMessagesOverlay(MessagesOverlay* messagesOverlay);

  MessageListener* messageListener() const;
  void setMessageListener(MessageListener* messageListener);

private:
  Q_DISABLE_COPY(MessageFeed)

  QString m_feedName;
  Message::MessageType m_feedMessageType = Message::MessageType::Unknown;
  QString m_feedMessageFormat;
  bool m_feedEnabled = true;
  MessagesOverlay* m_messagesOverlay = nullptr;
  MessageListener* m_messageListener = nullptr;
};

#endif // MESSAGEFEED_H
