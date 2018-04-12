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

#ifndef MESSAGEFEED_H
#define MESSAGEFEED_H

// Qt headers
#include <QObject>
#include <QUrl>

namespace Dsa {

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

  bool isFeedVisible() const;
  void setFeedVisible(bool feedVisible);

  MessagesOverlay* messagesOverlay() const;
  void setMessagesOverlay(MessagesOverlay* messagesOverlay);

  QUrl thumbnailUrl() const;
  void setThumbnailUrl(const QUrl& thumbnailUrl);

private:
  Q_DISABLE_COPY(MessageFeed)

  void updateOverlay();

  QString m_feedName;
  QString m_feedMessageType;
  bool m_feedVisible = true;
  MessagesOverlay* m_messagesOverlay = nullptr;
  QUrl m_thumbnailUrl;
};

} // Dsa

#endif // MESSAGEFEED_H
