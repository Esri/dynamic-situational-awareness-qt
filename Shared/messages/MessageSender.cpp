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

#include "MessageSender.h"

#include <QUdpSocket>

MessageSender::MessageSender(QObject* parent) :
  QObject(parent)
{
}

MessageSender::MessageSender(QIODevice* device, QObject *parent) :
  QObject(parent),
  m_device(device)
{
}

MessageSender::~MessageSender()
{
}

void MessageSender::setDevice(QIODevice* device)
{
  m_device = device;
}

QIODevice* MessageSender::device() const
{
  return m_device.data();
}

qint64 MessageSender::sendMessage(const QByteArray& message)
{
  // write the bytes to be sent to the device
  qint64 bytesWritten = m_device->write(message);
  if (bytesWritten != -1)
    emit messageSent(message);

  return bytesWritten;
}
