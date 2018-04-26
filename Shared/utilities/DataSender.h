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

#ifndef DATASENDER_H
#define DATASENDER_H

// Qt headers
#include <QObject>
#include <QPointer>

class QIODevice;

namespace Dsa {

class DataSender : public QObject
{
  Q_OBJECT

public:
  explicit DataSender(QObject* parent = nullptr);
  explicit DataSender(QIODevice* device, QObject* parent = nullptr);
  ~DataSender();

  void setDevice(QIODevice* device);
  QIODevice* device() const;

  qint64 sendData(const QByteArray& data);

signals:
  void dataSent(const QByteArray& data);

private:
  Q_DISABLE_COPY(DataSender)

  QPointer<QIODevice> m_device;
};

} // Dsa

#endif // DATASENDER_H
