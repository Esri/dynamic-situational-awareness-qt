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

class MessageFeedsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* messageFeeds READ messageFeeds CONSTANT)

public:
  static const QString RESOURCE_DIRECTORY_PROPERTYNAME;
  static const QString MESSAGE_FEED_UDP_PORTS_PROPERTYNAME;
  static const QString MESSAGE_FEEDS_PROPERTYNAME;

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

private:
  Esri::ArcGISRuntime::Renderer* createRenderer(const QString& rendererInfo, QObject* parent = nullptr) const;

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;

  MessageFeedListModel* m_messageFeeds = nullptr;
  QList<MessageListener*> m_messageListeners;
  QString m_dataPath;
};

#endif // MESSAGEFEEDSCONTROLLER_H
