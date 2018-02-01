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

#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include <QObject>
#include <QPointer>

class QIODevice;

class MessageSender : public QObject
{
  Q_OBJECT

public:
  explicit MessageSender(QObject* parent = nullptr);
  explicit MessageSender(QIODevice* device, QObject* parent = nullptr);
  ~MessageSender();

  void setDevice(QIODevice* device);
  QIODevice* device() const;

  qint64 sendMessage(const QByteArray& message);

signals:
  void messageSent(const QByteArray& message);

private:
  Q_DISABLE_COPY(MessageSender)

  QPointer<QIODevice> m_device;
};

#endif // MESSAGESENDER_H
