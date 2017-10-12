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

class MessageFeed : public QObject
{
  Q_OBJECT

public:
  explicit MessageFeed(QObject* parent = nullptr);
  MessageFeed(const QString& name, const QString& type, MessagesOverlay* overlay, QObject* parent = nullptr);
  ~MessageFeed() = default;

  QString feedName() const;
  void setFeedName(const QString& feedName);

  QString feedMessageType() const;
  void setFeedMessageType(const QString& feedMessageType);

  bool isFeedEnabled() const;
  void setFeedEnabled(bool feedEnabled);

  MessagesOverlay* messagesOverlay() const;
  void setMessagesOverlay(MessagesOverlay* messagesOverlay);

private:
  Q_DISABLE_COPY(MessageFeed)

  void updateOverlay();

  QString m_feedName;
  QString m_feedMessageType;
  bool m_feedEnabled = true;
  MessagesOverlay* m_messagesOverlay = nullptr;
};

#endif // MESSAGEFEED_H
