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

#ifndef DATALISTENER_H
#define DATALISTENER_H

// Qt headers
#include <QIODevice>
#include <QObject>
#include <QPointer>

namespace Dsa {

class DataListener : public QObject
{
  Q_OBJECT

public:
  explicit DataListener(QObject* parent = nullptr);
  explicit DataListener(QIODevice* device, QObject* parent = nullptr);
  ~DataListener();

  void setDevice(QIODevice* device);
  QIODevice* device() const;

  bool isEnabled() const;
  void setEnabled(bool enabled);

signals:
  void dataReceived(const QByteArray& data);

private:
  Q_DISABLE_COPY(DataListener)

  void connectDevice();
  void disconnectDevice();

  bool processUdpDatagrams();

  QPointer<QIODevice> m_device;
  QMetaObject::Connection m_deviceConn;

  bool m_enabled = true;
};

} // Dsa

#endif // DATALISTENER_H
