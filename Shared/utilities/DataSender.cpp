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

#include <QUdpSocket>

/*!
  \class DataSender
  \inherits QObject
  \brief Utility class for sending information over a UDP socket.
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
DataSender::DataSender(QObject* parent) :
  QObject(parent)
{
}

/*!
  \brief Constructor taking a QIODevice (\a device) and an optional \a parent.
 */
DataSender::DataSender(QIODevice* device, QObject *parent) :
  QObject(parent),
  m_device(device)
{
}

/*!
  \brief Destructor.
 */
DataSender::~DataSender()
{
}

void DataSender::setDevice(QIODevice* device)
{
  m_device = device;
}

/*!
  \brief Sets the QIODevice to \a device.
 */
QIODevice* DataSender::device() const
{
  return m_device.data();
}

/*!
  \brief Sends the QByteArray \a data with the current QIODevice.
 */
qint64 DataSender::sendData(const QByteArray& data)
{
  // write the bytes to be sent to the device
  qint64 bytesWritten = m_device->write(data);
  if (bytesWritten != -1)
    emit dataSent(data);

  return bytesWritten;
}
