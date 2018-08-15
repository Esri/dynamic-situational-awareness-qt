
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

#include "DataSender.h"

// Qt headers
#include <QUdpSocket>

namespace Dsa {

/*!
  \class Dsa::DataSender
  \inmodule Dsa
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

} // Dsa

// Signal Documentation
/*!
  \fn void DataSender::dataSent(const QByteArray& data);
  \brief Signal emitted when \a data is sent as a byte array.
 */
