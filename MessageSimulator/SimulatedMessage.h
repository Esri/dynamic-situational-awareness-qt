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

#ifndef SIMULATEDMESSAGE_H
#define SIMULATEDMESSAGE_H

#include <QObject>

class SimulatedMessage : public QObject
{
  Q_OBJECT

public:
  enum MessageFormat
  {
    CoT = 0,
    Unknown = -1
  };

  explicit SimulatedMessage(QObject* parent = nullptr);
  SimulatedMessage(MessageFormat messageFormat, const QString& messageId, const QString& messageAction, const QString& symbolId, QObject* parent = nullptr);
  ~SimulatedMessage();

  static SimulatedMessage* createFromCoTMessage(const QByteArray& message, QObject* parent = nullptr);

  MessageFormat messageFormat() const;
  void setMessageFormat(MessageFormat messageFormat);

  QString messageFormatString() const;

  QString messageId() const;
  void setMessageId(const QString& messageId);

  QString messageAction() const;
  void setMessageAction(const QString& messageAction);

  QString symbolId() const;
  void setSymbolId(const QString& symbolId);

private:
  Q_DISABLE_COPY(SimulatedMessage)

  MessageFormat m_messageFormat = MessageFormat::Unknown;
  QString m_messageId;
  QString m_messageAction;
  QString m_symbolId;
};

#endif // SIMULATEDMESSAGE_H
