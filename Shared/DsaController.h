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

#ifndef DSACONTROLLER_H
#define DSACONTROLLER_H

#include <QObject>

#include <QAbstractListModel>
#include <QUrl>

namespace Esri
{
namespace ArcGISRuntime
{
  class Error;
  class Scene;
  class GeoView;
namespace Toolkit
{
  class ArcGISCompassController;
}
}
}

class QUdpSocket;
class MessageListener;
class MessagesOverlay;

class DsaController : public QObject
{
  Q_OBJECT

public:
  DsaController(QObject* parent = nullptr);
  ~DsaController();

  Esri::ArcGISRuntime::Scene* scene() const;

  void init(Esri::ArcGISRuntime::GeoView* geoView);

public slots:
  void onError(const Esri::ArcGISRuntime::Error& error);

private:
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;

  QUdpSocket* m_udpSocket = nullptr;
  MessageListener* m_messageListener = nullptr;
  MessagesOverlay* m_messagesOverlay = nullptr;

  QString m_dataPath;
  int m_broadcastPort = 45678;
};

#endif // DSACONTROLLER_H
