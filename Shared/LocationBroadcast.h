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

#ifndef LOCATIONBROADCAST_H
#define LOCATIONBROADCAST_H

// dsa app headers
#include "Message.h"

namespace Esri::ArcGISRuntime {
  class Point;
}

// Qt headers
#include <QObject>

class QTimer;

namespace Dsa {

class Message;
class DataSender;

class LocationBroadcast : public QObject
{
  Q_OBJECT

public:
  explicit LocationBroadcast(QObject* parent = nullptr);
  LocationBroadcast(const QString& messageType, int udpPort, QObject* parent = nullptr);
  ~LocationBroadcast();

  bool isEnabled() const;
  void setEnabled(bool enabled);

  Esri::ArcGISRuntime::Point location() const;
  void setLocation(const Esri::ArcGISRuntime::Point& location);

  bool useCurrentLocation() const;
  void setUseCurrentLocation(bool useCurrentLocation);

  QString messageType() const;
  void setMessageType(const QString& messageType);

  int udpPort() const;
  void setUdpPort(int port);

  int frequency() const;
  void setFrequency(int frequency);

  bool isInDistress() const;
  void setInDistress(bool inDistress);

  Message message() const;

  QString userName() const;
  void setUserName(const QString& userName);

signals:
  void messageChanged();

private:
  Q_DISABLE_COPY(LocationBroadcast)

  void update();
  void broadcastLocation();
  void removeBroadcast();

  QString m_userName;
  bool m_enabled = true;
  Esri::ArcGISRuntime::Point m_location;
  bool m_useCurrentLocation = true;
  QString m_messageType;
  int m_udpPort = -1;
  int m_frequency = 3000;
  bool m_inDistress = false;

  DataSender* m_dataSender = nullptr;
  Message m_message;
  QTimer* m_timer = nullptr;

  QMetaObject::Connection m_locationChangedConn;
};

} // Dsa

#endif // LOCATIONBROADCAST_H
