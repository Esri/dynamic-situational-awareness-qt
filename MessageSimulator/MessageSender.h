#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include <QObject>
#include <QIODevice>
#include <QPointer>

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
