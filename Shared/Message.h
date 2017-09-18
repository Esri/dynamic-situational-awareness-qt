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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "Geometry.h"

#include <QSharedData>
#include <QVariantMap>

class MessageData;

class Message
{
public:
  enum class MessageAction
  {
    Create = 0,
    Update,
    Remove,
    Select,
    Unselect,
    Unknown = -1
  };

  Message();
  Message(const Message& other);
  Message(Message&& other);
  ~Message();

  Message& operator=(const Message& other);
  Message& operator=(Message&& other);

  static Message createFromCoTMessage(const QByteArray& message);
  static QString cotTypeToSidc(const QString& cotType);

  bool isEmpty() const;

  MessageAction messageAction() const;
  void setAction(MessageAction messageAction);

  QVariantMap attributes() const;
  void setAttributes(const QVariantMap& attributes);

  Esri::ArcGISRuntime::Geometry geometry() const;
  void setGeometry(const Esri::ArcGISRuntime::Geometry& geometry);

  QString messageId() const;
  void setMessageId(const QString& messageId);

  QString messageName() const;
  void setMessageName(const QString& messageName);

  QString messageType() const;
  void setMessageType(const QString& messageType);

  QString symbolId() const;
  void setSymbolId(const QString& symbolId);

private:
  QSharedDataPointer<MessageData> d;
};

#endif // MESSAGE_H
