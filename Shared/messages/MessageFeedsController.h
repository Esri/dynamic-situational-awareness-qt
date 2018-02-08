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

#ifndef MESSAGEFEEDSCONTROLLER_H
#define MESSAGEFEEDSCONTROLLER_H

#include "AbstractTool.h"

#include <QAbstractListModel>

namespace Esri {
  namespace ArcGISRuntime {
    class GeoView;
    class Renderer;
  }
}

class MessageFeedListModel;
class MessageListener;
class LocationBroadcast;

class MessageFeedsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* messageFeeds READ messageFeeds CONSTANT)
  Q_PROPERTY(bool locationBroadcastEnabled READ isLocationBroadcastEnabled WRITE setLocationBroadcastEnabled NOTIFY locationBroadcastEnabledChanged)
  Q_PROPERTY(int locationBroadcastFrequency READ locationBroadcastFrequency WRITE setLocationBroadcastFrequency NOTIFY locationBroadcastFrequencyChanged)

public:
  static const QString RESOURCE_DIRECTORY_PROPERTYNAME;

  explicit MessageFeedsController(QObject* parent = nullptr);
  ~MessageFeedsController();

  void setGeoView(Esri::ArcGISRuntime::GeoView* geoView);

  QAbstractListModel* messageFeeds() const;

  QList<MessageListener*> messageListeners() const;
  void addMessageListener(MessageListener* messageListener);
  void removeMessageListener(MessageListener* messageListener);

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  QString resourcePath() const { return m_resourcePath; }
  void setResourcePath(const QString& resourcePath);

  LocationBroadcast* locationBroadcast() const;

  bool isLocationBroadcastEnabled() const;
  void setLocationBroadcastEnabled(bool enabled);

  int locationBroadcastFrequency() const;
  void setLocationBroadcastFrequency(int frequency);

signals:
  void locationBroadcastEnabledChanged();
  void locationBroadcastFrequencyChanged();

private:
  Esri::ArcGISRuntime::Renderer* createRenderer(const QString& rendererInfo, QObject* parent = nullptr) const;

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;

  MessageFeedListModel* m_messageFeeds = nullptr;
  QList<MessageListener*> m_messageListeners;
  QString m_resourcePath;
  LocationBroadcast* m_locationBroadcast = nullptr;
};

#endif // MESSAGEFEEDSCONTROLLER_H
