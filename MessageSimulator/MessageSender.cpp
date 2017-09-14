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
