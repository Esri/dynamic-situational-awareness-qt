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

#include "DataSender.h"

// Qt headers
#include <QUdpSocket>

namespace Dsa {

DataSender::DataSender(QObject* parent) :
  QObject(parent)
{
}

DataSender::DataSender(QIODevice* device, QObject *parent) :
  QObject(parent),
  m_device(device)
{
}

DataSender::~DataSender()
{
}

void DataSender::setDevice(QIODevice* device)
{
  m_device = device;
}

QIODevice* DataSender::device() const
{
  return m_device.data();
}

qint64 DataSender::sendData(const QByteArray& data)
{
  // write the bytes to be sent to the device
  qint64 bytesWritten = m_device->write(data);
  if (bytesWritten != -1)
    emit dataSent(data);

  return bytesWritten;
}

} // Dsa
