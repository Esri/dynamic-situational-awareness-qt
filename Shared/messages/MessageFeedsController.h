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

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>
#include <QVariantList>

namespace Esri {
  namespace ArcGISRuntime {
    class GeoView;
    class Renderer;
    enum class SurfacePlacement;
  }
}

namespace Dsa {

class DataListener;

class LocationBroadcast;

class MessageFeedListModel;

class MessageFeedsController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* messageFeeds READ messageFeeds CONSTANT)
  Q_PROPERTY(bool locationBroadcastEnabled READ isLocationBroadcastEnabled WRITE setLocationBroadcastEnabled NOTIFY locationBroadcastEnabledChanged)
  Q_PROPERTY(int locationBroadcastFrequency READ locationBroadcastFrequency WRITE setLocationBroadcastFrequency NOTIFY locationBroadcastFrequencyChanged)
  Q_PROPERTY(bool locationBroadcastInDistress READ isLocationBroadcastInDistress WRITE setLocationBroadcastInDistress NOTIFY locationBroadcastInDistressChanged)

public:
  static const QString RESOURCE_DIRECTORY_PROPERTYNAME;

  explicit MessageFeedsController(QObject* parent = nullptr);
  ~MessageFeedsController();

  void setGeoView(Esri::ArcGISRuntime::GeoView* geoView);

  QAbstractListModel* messageFeeds() const;

  QList<DataListener*> dataListeners() const;
  void addDataListener(DataListener* dataListener);
  void removeDataListener(DataListener* dataListener);

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  QString resourcePath() const { return m_resourcePath; }
  void setResourcePath(const QString& resourcePath);

  LocationBroadcast* locationBroadcast() const;

  bool isLocationBroadcastEnabled() const;
  void setLocationBroadcastEnabled(bool enabled);

  int locationBroadcastFrequency() const;
  void setLocationBroadcastFrequency(int frequency);

  bool isLocationBroadcastInDistress() const;
  void setLocationBroadcastInDistress(bool inDistress);

  static Esri::ArcGISRuntime::SurfacePlacement toSurfacePlacement(const QString& surfacePlacement);

signals:
  void locationBroadcastEnabledChanged();
  void locationBroadcastFrequencyChanged();
  void locationBroadcastInDistressChanged();
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

private:
  void setupFeeds();
  Esri::ArcGISRuntime::Renderer* createRenderer(const QString& rendererInfo, QObject* parent = nullptr) const;

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;

  MessageFeedListModel* m_messageFeeds = nullptr;
  QList<DataListener*> m_dataListeners;
  QString m_resourcePath;
  LocationBroadcast* m_locationBroadcast = nullptr;
  QVariantList m_messageFeedProperties;
};

} // Dsa

#endif // MESSAGEFEEDSCONTROLLER_H
