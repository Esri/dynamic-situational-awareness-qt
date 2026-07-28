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

// C++ API
#include "DynamicEntityDataSource.h"
#include "Field.h"
// Qt
#include <QHash>
#include <QObject>
#include <QUrl>
#include <QVariantMap>

namespace Esri::ArcGISRuntime {

class DictionarySymbolStyle;
class DynamicEntityDataSourceInfo;
class DynamicEntity;
class Renderer;

}

namespace Dsa {

class Message;
class MessagesOverlay;

class MessageFeed : public Esri::ArcGISRuntime::DynamicEntityDataSource
{
  Q_OBJECT

  Q_PROPERTY(bool showPreviousObservations READ showPreviousObservations WRITE setShowPreviousObservations NOTIFY feedChanged)
  Q_PROPERTY(QString colorObservations READ colorObservations WRITE setColorObservations NOTIFY feedChanged)
  Q_PROPERTY(int sizeObservations READ sizeObservations WRITE setSizeObservations NOTIFY feedChanged)
  Q_PROPERTY(bool showTrackLine READ showTrackLine WRITE setShowTrackLine NOTIFY feedChanged)
  Q_PROPERTY(QString colorTrackLine READ colorTrackLine WRITE setColorTrackLine NOTIFY feedChanged)
  Q_PROPERTY(int sizeTrackLine READ sizeTrackLine WRITE setSizeTrackLine NOTIFY feedChanged)
  Q_PROPERTY(int maximumObservations READ maximumObservations WRITE setMaximumObservations NOTIFY feedChanged)

public:
  MessageFeed(const QVariantMap& properties, const QString& resourcePath, QObject* parent = nullptr);
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

  QString thumbnail() const;
  QUrl thumbnailUrl() const;

  QString renderer() const;
  QString surfacePlacement() const;

  bool addMessage(const Message& message);
  Esri::ArcGISRuntime::DynamicEntity* getDynamicEntityById(quint64 entityId) const;

  const QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*>& dynamicEntities() const;

  bool showPreviousObservations() const;
  int maximumObservations() const;
  QString colorObservations() const;
  int sizeObservations() const;
  bool showTrackLine() const;
  QString colorTrackLine() const;
  int sizeTrackLine() const;
  QString searchAttributeName() const;
  QString entityIdAttributeName() const;
  bool configurationWasValid() const;

signals:
  void feedChanged();

private:
  Q_DISABLE_COPY(MessageFeed)

  inline static Esri::ArcGISRuntime::DictionarySymbolStyle* s_dictionarySymbolStyleMil2525c = nullptr;
  inline static Esri::ArcGISRuntime::DictionarySymbolStyle* s_dictionarySymbolStyleMil2525d = nullptr;

  void setFields(const QString& schemaUrl);

  Esri::ArcGISRuntime::Renderer* createRenderer();
  void setThumbnailUrl(const QUrl& thumbnailUrl);
  void setShowPreviousObservations(bool showPreviousObservations);
  void setMaximumObservations(int maximumObservations);
  void setColorObservations(const QString& color);
  void setSizeObservations(int symbolSize);
  void setShowTrackLine(bool showTrackLine);
  void setColorTrackLine(const QString& color);
  void setSizeTrackLine(int symbolSize);
  void updateSymbolObservations();
  void updateSymbolTrackLine();

  QString m_feedName;
  QString m_feedMessageType;
  QString m_renderer;
  QString m_thumbnail;
  QString m_surfacePlacement;
  bool m_isFeedVisible = true;
  bool m_isCoT = false;
  bool m_configurationWasValid = false;
  bool m_showPreviousObservations = false;
  int m_maximumObservations = 5;
  QString m_colorObservations;
  int m_sizeObservations = 10;
  bool m_showTrackLine = false;
  QString m_colorTrackLine;
  int m_sizeTrackLine = 4;
  QString m_entityIdAttributeName;
  QString m_searchAttributeName;
  MessagesOverlay* m_messagesOverlay = nullptr;
  QUrl m_thumbnailUrl;
  QString m_resourcePath;
  QList<Esri::ArcGISRuntime::Field> m_fields;
  QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*> m_dynamicEntities;
  void checkEntityForSelectAction(Esri::ArcGISRuntime::DynamicEntity* dynamicEntity);
};

} // Dsa

#endif // MESSAGEFEED_H
