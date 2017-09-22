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

#include "MessageListener.h"

#include <QUdpSocket>

MessageListener::MessageListener(QObject* parent) :
  QObject(parent)
{
}

MessageListener::MessageListener(QIODevice* device, QObject *parent) :
  QObject(parent),
  m_device(device)
{
  connectDevice();
}

MessageListener::~MessageListener()
{
  disconnectDevice();
}

void MessageListener::setDevice(QIODevice* device)
{
  disconnectDevice();

  m_device = device;
  connectDevice();
}

QIODevice* MessageListener::device() const
{
  return m_device.data();
}

bool MessageListener::isEnabled() const
{
  return m_enabled;
}

void MessageListener::setEnabled(bool enabled)
{
  if (m_enabled == enabled)
    return;

  if (!m_enabled && enabled)
    connectDevice();
  else if (m_enabled && !enabled)
    disconnectDevice();

  m_enabled = enabled;
}

void MessageListener::connectDevice()
{
  disconnectDevice();

  m_deviceConn = connect(m_device.data(), &QIODevice::readyRead, this, [this]
  {
    if (!processUdpDatagrams())
    {
      // if bytes were not processed as UDP datagram then
      // read bytes directly from the device
      emit messageReceived(m_device->readAll());
    }
  });
}

void MessageListener::disconnectDevice()
{
  if (m_deviceConn)
    disconnect(m_deviceConn);

  // any pending UDP datagrams need to be processed or else
  // QIODevice::readyRead() signal is not emitted for the next datagram
  processUdpDatagrams();
}

bool MessageListener::processUdpDatagrams()
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
      emit messageReceived(datagram.data());
    }

    return true;
  }

  return false;
}
