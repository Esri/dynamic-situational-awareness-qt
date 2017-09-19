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

#ifndef MESSAGELISTENER_H
#define MESSAGELISTENER_H

#include <QObject>
#include <QIODevice>
#include <QPointer>

class MessageListener : public QObject
{
  Q_OBJECT

public:
  explicit MessageListener(QObject* parent = nullptr);
  explicit MessageListener(QIODevice* device, QObject* parent = nullptr);
  ~MessageListener();

  void setDevice(QIODevice* device);
  QIODevice* device() const;

signals:
  void messageReceived(const QByteArray& message);

private:
  Q_DISABLE_COPY(MessageListener)

  void initDevice();

  QPointer<QIODevice> m_device;
  QMetaObject::Connection m_deviceConn;
};

#endif // MESSAGELISTENER_H
