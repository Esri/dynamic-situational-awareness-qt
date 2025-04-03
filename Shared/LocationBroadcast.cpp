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

// PCH header
#include "pch.hpp"

#include "LocationBroadcast.h"

// dsa app headers
#include "DataSender.h"

// toolkit headers
#include "ToolResourceProvider.h"

// Qt headers
#include <QHostInfo>
#include <QTimer>
#include <QUdpSocket>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

// friendly symbol ID for our location broadcast
static const QString s_locationBroadcastSic{QStringLiteral("SFGPEVAL-------")};

/*!
  \class Dsa::LocationBroadcast
  \inmodule Dsa
  \inherits QObject
  \brief Broadcasts location updates over a specified message type.

  The LocationBroadcast requires that a message type and UDP port be specified
  to be able to broadcast location update messages.

  The broadcast should typically be configured with an existing message feed type
  over an existing message feed UDP port.

  \sa MessageFeedsController
  \sa setMessageType
  \sa setUdpPort
 */


/*!
   \brief Constructs a default LocationBroadcast object with
   an optional \a parent.

   The message type and UDP port must be specified to broadcast location updates.

   \sa setMessageType
   \sa setUdpPort
 */
LocationBroadcast::LocationBroadcast(QObject* parent) :
  QObject(parent),
  m_userName(QHostInfo::localHostName())
{
}

/*!
   \brief Constructs a LocationBroadcast object with \a messageType, \a udpPort,
   and optional \a parent.
 */
LocationBroadcast::LocationBroadcast(const QString& messageType, int udpPort, QObject* parent) :
  QObject(parent),
  m_userName(QHostInfo::localHostName()),
  m_messageType(messageType),
  m_udpPort(udpPort)
{
  update();
}

/*!
   \brief Destructor
 */
LocationBroadcast::~LocationBroadcast()
{
  removeBroadcast();
}

/*!
   \brief Returns \c true if the location broadcast is enabled.
 */
bool LocationBroadcast::isEnabled() const
{
  return m_enabled;
}

/*!
   \brief Sets whether the location broadcast is \a enabled.

   The broadcast will stop sending updates if \a enabled is \c false.
 */
void LocationBroadcast::setEnabled(bool enabled)
{
  if (m_enabled == enabled)
    return;

  m_enabled = enabled;

  update();
}

/*!
   \brief Returns the current location of the location broadcast.
 */
Point LocationBroadcast::location() const
{
  return m_location;
}

/*!
   \brief Sets the current location to \a location and updates
   the location broadcast.
 */
void LocationBroadcast::setLocation(const Point& location)
{
  if (m_useCurrentLocation)
    return;

  m_location = location;

  broadcastLocation();
}

/*!
   \brief Returns \c true if the location broadcast
   uses the current location of the device or simulated location.

   By default, this value is \c true.
 */
bool LocationBroadcast::useCurrentLocation() const
{
  return m_useCurrentLocation;
}

/*!
   \brief Sets whether the location broadcast will use the device
   or simulated location to \a useCurrentLocation.
 */
void LocationBroadcast::setUseCurrentLocation(bool useCurrentLocation)
{
  if (m_useCurrentLocation == useCurrentLocation)
    return;

  m_useCurrentLocation = useCurrentLocation;

  update();
}

/*!
   \brief Returns the message type of the location broadcast.
 */
QString LocationBroadcast::messageType() const
{
  return m_messageType;
}

/*!
   \brief Sets the message type of the location broadcast to \a messageType.

   Setting a new message type will update and start the location broadcast if enabled.
 */
void LocationBroadcast::setMessageType(const QString& messageType)
{
  if (m_messageType == messageType)
    return;

  m_messageType = messageType;

  if (m_messageType.isEmpty())
    setEnabled(false);
  else
    update();
}

/*!
   \brief Returns the UDP port used by the location broadcast.
 */
int LocationBroadcast::udpPort() const
{
  return m_udpPort;
}

/*!
   \brief Sets the UDP port used by the location broadcast to \a port.

   Setting a new UDP port will update and start the location broadcast if enabled.
 */
void LocationBroadcast::setUdpPort(int port)
{
  if (m_udpPort == port)
    return;

  m_udpPort = port;

  if (m_udpPort == -1)
    setEnabled(false);
  else
    update();
}

/*!
   \brief Returns the frequency of broadcasted location updates.

   The frequency value is in milliseconds. The default is \c 3000 milliseconds
 */
int LocationBroadcast::frequency() const
{
  return m_frequency;
}

/*!
   \brief Sets the frequency, in milliseconds, of broadcasted location updates to \a frequency.

   Setting the frequency to a new value will adjust the current broadcast of
   location updates.

   The default is \c 3000 milliseconds
 */
void LocationBroadcast::setFrequency(int frequency)
{
  if (m_frequency== frequency)
    return;

  m_frequency = frequency;

  if (m_timer)
    m_timer->setInterval(m_frequency);
}

/*!
   \brief Returns \c true if the location broadcast reports
   message status as being in distress.

   The default is \c false

   \sa message
 */
bool LocationBroadcast::isInDistress() const
{
  return m_inDistress;
}

/*!
   \brief Sets the location broadcast to report
   message status as being in distress to \a inDistress.

   Setting \a inDistress to \c true will enable the
   location broadcast if disabled.
 */
void LocationBroadcast::setInDistress(bool inDistress)
{
  if (m_inDistress == inDistress)
    return;

  m_inDistress = inDistress;

  if (m_inDistress && !isEnabled())
    setEnabled(true);
}

/*!
   \brief Returns the message that is being broadcasted.
 */
Message LocationBroadcast::message() const
{
  return m_message;
}

/*!
   \internal
   \brief Updates the configuration of the location broadcast to use
   the specified message feed type and UDP port.

   The existing broadcast will be stopped and torn down. If enabled, a new location broadcast
   will be started at the specified frequency rate.
 */
void LocationBroadcast::update()
{
  if (m_locationChangedConn)
    disconnect(m_locationChangedConn);

  if (m_messageType.isEmpty() || m_udpPort == -1)
    return;

  if (m_dataSender)
  {
    delete m_dataSender;
    m_dataSender = nullptr;
    m_timer = nullptr;
  }

  m_dataSender = new DataSender(this);

  QUdpSocket* udpSocket = new QUdpSocket(m_dataSender);
  udpSocket->connectToHost(QHostAddress::Broadcast, m_udpPort, QIODevice::WriteOnly);
  m_dataSender->setDevice(udpSocket);

  m_timer = new QTimer(m_dataSender);
  connect(m_timer, &QTimer::timeout, this, [this]
  {
    broadcastLocation();
  });

  if (m_enabled)
    m_timer->start(m_frequency);

  if (m_useCurrentLocation)
  {
    m_locationChangedConn = connect(ToolResourceProvider::instance(), &ToolResourceProvider::locationChanged,
                                    this, [this](const Point& location)
    {
      if (!m_enabled)
        return;

      m_location = location;
    });
  }
}

/*!
   \internal
   \brief Broadcasts the current location with the configured
   message feed type and UDP port.

   This function updates the current message with the current location
   and broadcasts the message in GeoMessage format.
 */
void LocationBroadcast::broadcastLocation()
{
  if (!m_enabled || !m_dataSender || m_location.isEmpty())
    return;

  if (m_message.isEmpty())
  {
    QVariantMap attribs;

    m_message = Message(Message::MessageAction::Update, m_location);
    m_message.setMessageId(QUuid::createUuid().toString());
    m_message.setMessageType(m_messageType);
    m_message.setSymbolId(s_locationBroadcastSic);

    attribs.insert(Message::GEOMESSAGE_SIC_NAME, s_locationBroadcastSic);
    attribs.insert(Message::GEOMESSAGE_UNIQUE_DESIGNATION_NAME, m_userName);
    const int status911 = m_inDistress ? 1 : 0;
    attribs.insert(Message::GEOMESSAGE_STATUS_911_NAME, status911);
    m_message.setAttributes(attribs);
  }
  else
  {
    m_message.setGeometry(m_location);

    QVariantMap attribs = m_message.attributes();

    const int status911 = m_inDistress ? 1 : 0;
    attribs.insert(Message::GEOMESSAGE_STATUS_911_NAME, status911);
    m_message.setAttributes(attribs);
  }

  emit messageChanged();

  m_dataSender->sendData(m_message.toGeoMessage());
}

/*!
   \internal
   \brief Removes the location broadcast by broadcasting
   a message with the Message::MessageAction::Remove action.

   This function is called at the time of the object's destruction.
 */
void LocationBroadcast::removeBroadcast()
{
  if (!m_message.isEmpty())
  {
    m_message.setMessageAction(Message::MessageAction::Remove);

    emit messageChanged();

    if (m_dataSender)
      m_dataSender->sendData(m_message.toGeoMessage());
  }
}

/*!
   \brief Returns the user name for the location broadcast.
 */
QString LocationBroadcast::userName() const
{
  return m_userName;
}

/*!
   \brief Sets the user name for the location broadcast to \a userName.
 */
void LocationBroadcast::setUserName(const QString& userName)
{
  if (userName == m_userName)
    return;

  m_userName = userName;

  if (!m_message.isEmpty())
  {
    QVariantMap attribs = m_message.attributes();
    attribs.insert(Message::GEOMESSAGE_UNIQUE_DESIGNATION_NAME, m_userName);
    m_message.setAttributes(attribs);
  }
}

// Signal Documentation
/*!
  \fn void LocationBroadcast::messageChanged();
  \brief Signal emitted when the message for this location broadcast changes.
 */

} // Dsa
