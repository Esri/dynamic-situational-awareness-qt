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

#ifndef DATASENDER_H
#define DATASENDER_H

#include <QObject>
#include <QPointer>

class QIODevice;

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

#endif // DATASENDER_H
