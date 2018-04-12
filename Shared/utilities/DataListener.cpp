// Copyright 2018 ESRI
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

// PCH header
#include "pch.hpp"

#include "DataListener.h"

// Qt headers
#include <QUdpSocket>

namespace Dsa {

/*!
  \class DataListener
  \inherits QObject
  \brief Utility class for listening on a UDP socket.
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
DataListener::DataListener(QObject* parent) :
  QObject(parent)
{
}

/*!
  \brief Constructor taking a QIODevice (\a device) and an optional \a parent.
 */
DataListener::DataListener(QIODevice* device, QObject *parent) :
  QObject(parent),
  m_device(device)
{
  connectDevice();
}

/*!
  \brief Destructor.
 */
DataListener::~DataListener()
{
  disconnectDevice();
}

/*!
  \brief Sets the QIODevice to \a device.
 */
void DataListener::setDevice(QIODevice* device)
{
  disconnectDevice();

  m_device = device;
  connectDevice();
}

/*!
  \brief Returns the current QIODevice.
 */
QIODevice* DataListener::device() const
{
  return m_device.data();
}

/*!
  \brief Returns whether the data listener is enabled.
 */
bool DataListener::isEnabled() const
{
  return m_enabled;
}

/*!
  \brief Sets the data listener to be \a enabled.
 */
void DataListener::setEnabled(bool enabled)
{
  if (m_enabled == enabled)
    return;

  if (!m_enabled && enabled)
    connectDevice();
  else if (m_enabled && !enabled)
    disconnectDevice();

  m_enabled = enabled;
}

/*!
  \internal
 */
void DataListener::connectDevice()
{
  disconnectDevice();

  if (!m_device)
    return;

  m_deviceConn = connect(m_device.data(), &QIODevice::readyRead, this, [this]
  {
    if (!processUdpDatagrams())
    {
      // if bytes were not processed as UDP datagram then
      // read bytes directly from the device
      emit dataReceived(m_device->readAll());
    }
  });
}

/*!
  \internal
 */
void DataListener::disconnectDevice()
{
  if (m_deviceConn)
    disconnect(m_deviceConn);

  // any pending UDP datagrams need to be processed or else
  // QIODevice::readyRead() signal is not emitted for the next datagram
  processUdpDatagrams();
}

/*!
  \internal
 */
bool DataListener::processUdpDatagrams()
{
  QUdpSocket* udpSocket = qobject_cast<QUdpSocket*>(m_device);
  if (udpSocket)
  {
    // there is currently a Qt limitation that the listener needs to call
    // the QUdpSocket datagram methods instead of being able to use
    // QIODevice's readAll() method directly.
    while (udpSocket->hasPendingDatagrams())
    {
      QByteArray datagram;
      datagram.resize(udpSocket->pendingDatagramSize());
      udpSocket->readDatagram(datagram.data(), datagram.size());
      emit dataReceived(datagram.data());
    }

    return true;
  }

  return false;
}

} // Dsa
