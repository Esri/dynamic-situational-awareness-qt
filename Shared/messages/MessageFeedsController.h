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
#include "Message.h"
#include "Point.h"

#include <QAbstractListModel>

namespace Esri {
  namespace ArcGISRuntime {
    class GeoView;
    class Renderer;
  }
}

class MessageFeedListModel;
class MessageListener;
class MessageSender;
class QTimer;

class MessageFeedsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* messageFeeds READ messageFeeds CONSTANT)
  Q_PROPERTY(bool sendLocationMessageEnabled READ isSendLocationMessageEnabled WRITE setSendLocationMessageEnabled NOTIFY sendLocationMessageEnabledChanged)
  Q_PROPERTY(int sendLocationMessageFrequency READ sendLocationMessageFrequency WRITE setSendLocationMessageFrequency NOTIFY sendLocationMessageFrequencyChanged)

public:
  static const QString RESOURCE_DIRECTORY_PROPERTYNAME;
  static const QString MESSAGE_FEED_UDP_PORTS_PROPERTYNAME;
  static const QString MESSAGE_FEEDS_PROPERTYNAME;
  static const QString SEND_LOCATION_MESSAGE_FEED_PROPERTYNAME;

  explicit MessageFeedsController(QObject* parent = nullptr);
  ~MessageFeedsController();

  void setGeoView(Esri::ArcGISRuntime::GeoView* geoView);

  QAbstractListModel* messageFeeds() const;

  QList<MessageListener*> messageListeners() const;
  void addMessageListener(MessageListener* messageListener);
  void removeMessageListener(MessageListener* messageListener);

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  QString dataPath() const { return m_dataPath; }
  void setDataPath(const QString& dataPath);

  QString sendLocationMessageFeedType() const;
  void setSendLocationMessageFeedType(const QString& messageFeedType);

  int sendLocationMessageFeedUdpPort() const;
  void setSendLocationMessageFeedUdpPort(int port);

  bool isSendLocationMessageEnabled() const;
  void setSendLocationMessageEnabled(bool enabled);

  int sendLocationMessageFrequency() const;
  void setSendLocationMessageFrequency(int frequency);

signals:
  void sendLocationMessageEnabledChanged();
  void sendLocationMessageFrequencyChanged();

private:
  Esri::ArcGISRuntime::Renderer* createRenderer(const QString& rendererInfo, QObject* parent = nullptr) const;
  void updateSendLocationMessage();

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;

  MessageFeedListModel* m_messageFeeds = nullptr;
  QList<MessageListener*> m_messageListeners;
  QString m_dataPath;
  QString m_sendLocationMessageFeedType;
  int m_sendLocationMessageFeedUdpPort = -1;
  bool m_sendLocationMessageEnabled = true;
  MessageSender* m_sendLocationMessageSender = nullptr;
  Message m_sendLocationMessage;
  QTimer* m_sendLocationMessageTimer = nullptr;
  int m_sendLocationMessageFrequency = 3000;
  Esri::ArcGISRuntime::Point m_currentLocation;

  QMetaObject::Connection m_locationChangedConn;
};

#endif // MESSAGEFEEDSCONTROLLER_H
