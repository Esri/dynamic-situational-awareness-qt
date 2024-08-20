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

#ifndef MESSAGEFEED_H
#define MESSAGEFEED_H

// Qt headers
#include <QObject>

// MapsSDK headers
#include "DynamicEntityDataSource.h"

namespace Esri::ArcGISRuntime {

class DynamicEntityDataSourceInfo;
class DynamicEntity;

}

namespace Dsa {

class Message;
class MessagesOverlay;

class MessageFeed : public Esri::ArcGISRuntime::DynamicEntityDataSource
{
  Q_OBJECT

public:
  MessageFeed(const QString& name, const QString& type, QObject* parent = nullptr);
  ~MessageFeed() override;

  QFuture<Esri::ArcGISRuntime::DynamicEntityDataSourceInfo*> onLoadAsync() override;
  QFuture<void> onConnectAsync() override;
  QFuture<void> onDisconnectAsync() override;

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

  bool addMessage(const Message& message);
  Esri::ArcGISRuntime::DynamicEntity* getDynamicEntityById(quint64 entityId);

  const QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*>& dynamicEntities();

private:
  Q_DISABLE_COPY(MessageFeed)

  QString m_feedName;
  QString m_feedMessageType;
  bool m_isCoT;
  QString m_entityIdAttributeName;
  MessagesOverlay* m_messagesOverlay = nullptr;
  QUrl m_thumbnailUrl;
  QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*> m_dynamicEntities;
};

} // Dsa

#endif // MESSAGEFEED_H
