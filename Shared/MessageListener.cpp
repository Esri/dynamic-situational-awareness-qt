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
  initDevice();
}

MessageListener::~MessageListener()
{
}

void MessageListener::setDevice(QIODevice* device)
{
  m_device = device;
  initDevice();
}

QIODevice* MessageListener::device() const
{
  return m_device.data();
}

void MessageListener::initDevice()
{
  if (m_deviceConn)
    disconnect(m_deviceConn);

  m_deviceConn = connect(m_device.data(), &QIODevice::readyRead, this, [this]
  {
    QUdpSocket* udpSocket = qobject_cast<QUdpSocket*>(m_device);
    if (udpSocket)
    {
      // there is currently a Qt limitation that the listener needs to call
      // the QUdpSocket methods for some reason instead of being able to use
      // QIODevice's readAll() method.
      while (udpSocket->hasPendingDatagrams())
      {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        emit messageReceived(datagram.data());
      }
    }
    else
    {
      // Read bytes from the device
      emit messageReceived(m_device->readAll());
    }
  });
}
