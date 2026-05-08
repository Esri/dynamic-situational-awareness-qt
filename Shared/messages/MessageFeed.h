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
// Qt
#include <QHash>
#include <QObject>
#include <QUrl>
#include <QVariantMap>

namespace Esri::ArcGISRuntime {

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
  // Q_PROPERTY(int maximumDuration READ maximumDuration WRITE setMaximumDuration NOTIFY feedChanged)
  // Q_PROPERTY(QString maximumDurationUnits READ maximumDurationUnits WRITE setMaximumDurationUnits NOTIFY feedChanged)

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
  void setThumbnailUrl(const QUrl& thumbnailUrl);

  QString renderer() const;
  QString surfacePlacement() const;

  Esri::ArcGISRuntime::Renderer* createRenderer();

  bool addMessage(const Message& message);
  Esri::ArcGISRuntime::DynamicEntity* getDynamicEntityById(quint64 entityId) const;

  const QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*>& dynamicEntities() const;

  bool showPreviousObservations() const;
  void setShowPreviousObservations(bool showPreviousObservations);
  int maximumObservations() const;
  void setMaximumObservations(int maximumObservations);
  QString colorObservations() const;
  void setColorObservations(const QString& color);
  int sizeObservations() const;
  void setSizeObservations(int symbolSize);
  void updateSymbolObservations();
  bool showTrackLine() const;
  void setShowTrackLine(bool showTrackLine);
  QString colorTrackLine() const;
  void setColorTrackLine(const QString& color);
  int sizeTrackLine() const;
  void setSizeTrackLine(int symbolSize);
  void updateSymbolTrackLine();
  // int maximumDuration() const;
  // void setMaximumDuration(int duration);
  // QString maximumDurationUnits() const;
  // void setMaximumDurationUnits(const QString& units);
  // void updateDuration();

  void setupOverlay();
  bool configurationWasValid() const;

signals:
  void feedChanged();

private:
  Q_DISABLE_COPY(MessageFeed)

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
  MessagesOverlay* m_messagesOverlay = nullptr;
  QUrl m_thumbnailUrl;
  QString m_resourcePath;
  int m_maximumDuration = 0;
  QString m_maximumDurationUnits;
  QHash<quint64, Esri::ArcGISRuntime::DynamicEntity*> m_dynamicEntities;
  void checkEntityForSelectAction(Esri::ArcGISRuntime::DynamicEntity* dynamicEntity);
};

} // Dsa

#endif // MESSAGEFEED_H
