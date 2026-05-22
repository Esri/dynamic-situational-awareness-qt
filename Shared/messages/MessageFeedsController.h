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

#ifndef MESSAGEFEEDSCONTROLLER_H
#define MESSAGEFEEDSCONTROLLER_H

// DSA
#include "AbstractTool.h"
#include "MessageFeed.h"
// Qt
#include <QStringListModel>
#include <QList>
#include <QVariantList>

Q_MOC_INCLUDE("qabstractitemmodel.h")

class QAbstractListModel;

namespace Esri::ArcGISRuntime {
  class Renderer;
  class Scene;
}

namespace Dsa {

class DataListener;
class LocationBroadcast;
class MessageFeedListModel;

class MessageFeedsController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* messageFeeds READ messageFeeds CONSTANT)
  Q_PROPERTY(QAbstractListModel* entityIdResults READ entityIdResults CONSTANT)
  Q_PROPERTY(bool locationBroadcastEnabled READ isLocationBroadcastEnabled WRITE setLocationBroadcastEnabled NOTIFY locationBroadcastEnabledChanged)
  Q_PROPERTY(int locationBroadcastFrequency READ locationBroadcastFrequency WRITE setLocationBroadcastFrequency NOTIFY locationBroadcastFrequencyChanged)
  Q_PROPERTY(bool locationBroadcastInDistress READ isLocationBroadcastInDistress WRITE setLocationBroadcastInDistress NOTIFY locationBroadcastInDistressChanged)
  Q_PROPERTY(int selectedFeedIndex READ selectedFeedIndex WRITE setSelectedFeedIndex NOTIFY selectedFeedChanged)
  Q_PROPERTY(MessageFeed* selectedFeed READ selectedFeed NOTIFY selectedFeedChanged);

public:
  static const QString RESOURCE_DIRECTORY_PROPERTYNAME;

  Q_INVOKABLE void findEntities(const QString& entityIdText);
  Q_INVOKABLE void selectEntity(int index);
  Q_INVOKABLE void clearSearchResults();

  explicit MessageFeedsController(QObject* parent = nullptr);
  ~MessageFeedsController();

  QAbstractListModel* messageFeeds() const;
  QAbstractListModel* entityIdResults() const;

  QList<DataListener*> dataListeners() const;
  void addDataListener(DataListener* dataListener);
  void removeDataListener(DataListener* dataListener);

  QString toolName() const override;
  void toolInitProperties(const QVariantMap& properties) override;
  bool shouldSetProperties(const QString& propertyName) override;

  QString resourcePath() const { return m_resourcePath; }
  void setResourcePath(const QString& resourcePath);

  LocationBroadcast* locationBroadcast() const;

  bool isLocationBroadcastEnabled() const;
  void setLocationBroadcastEnabled(bool enabled);

  int locationBroadcastFrequency() const;
  void setLocationBroadcastFrequency(int frequency);

  bool isLocationBroadcastInDistress() const;
  void setLocationBroadcastInDistress(bool inDistress);

  MessageFeed* selectedFeed();

  int selectedFeedIndex() const;
  void setSelectedFeedIndex(int index);

  void notifyPropertyChanged();

signals:
  void locationBroadcastEnabledChanged();
  void locationBroadcastFrequencyChanged();
  void locationBroadcastInDistressChanged();
  void selectedFeedChanged();
  void entitySelected(const QString& entityId, MessageFeed* messageFeed);

  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

private:
  void setSceneFromGeoView();
  void setupFeeds();

  Esri::ArcGISRuntime::Scene* m_scene = nullptr;

  MessageFeedListModel* m_messageFeeds = nullptr;
  QList<DataListener*> m_dataListeners;
  QString m_resourcePath;
  LocationBroadcast* m_locationBroadcast = nullptr;
  QVariantList m_messageFeedProperties;
  int m_selectedFeedIndex = -1;
  QStringListModel* m_entityIdResults = nullptr;
  std::vector<QString> m_entityIds;
};

} // Dsa

#endif // MESSAGEFEEDSCONTROLLER_H
