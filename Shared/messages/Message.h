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
  static const QString COT_ROOT_ELEMENT_NAME;
  static const QString COT_ELEMENT_NAME;
  static const QString COT_TYPE_NAME;
  static const QString COT_UID_NAME;
  static const QString COT_POINT_NAME;
  static const QString COT_POINT_LAT_NAME;
  static const QString COT_POINT_LON_NAME;
  static const QString COT_POINT_HAE_NAME;

  static const QString GEOMESSAGE_ROOT_ELEMENT_NAME;
  static const QString GEOMESSAGE_ELEMENT_NAME;
  static const QString GEOMESSAGE_TYPE_NAME;
  static const QString GEOMESSAGE_ACTION_NAME;
  static const QString GEOMESSAGE_ID_NAME;
  static const QString GEOMESSAGE_WKID_NAME;
  static const QString GEOMESSAGE_SIC_NAME;
  static const QString GEOMESSAGE_CONTROL_POINTS_NAME;
  static const QString GEOMESSAGE_UNIQUE_DESIGNATION_NAME;
  static const QString GEOMESSAGE_STATUS_911_NAME;
  static const QString GEOMESSAGE_ENVIRONMENT_NAME;

  static const QString SIDC_NAME;

  enum class MessageAction
  {
    Update = 0,
    Remove,
    Select,
    Unselect,
    Unknown = -1
  };

  Message();
  Message(MessageAction messageAction, const Esri::ArcGISRuntime::Geometry& geometry);
  Message(const Message& other);
  Message(Message&& other);
  ~Message();

  Message& operator=(const Message& other);
  Message& operator=(Message&& other);

  bool operator==(const Message& other) const;

  static Message create(const QByteArray& message);
  static Message createFromCoTMessage(const QByteArray& message);
  static Message createFromGeoMessage(const QByteArray& message);

  static QString cotTypeToSidc(const QString& cotType);
  static MessageAction toMessageAction(const QString& action);
  static QString fromMessageAction(MessageAction action);

  bool isEmpty() const;

  MessageAction messageAction() const;
  void setMessageAction(MessageAction messageAction);

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

  QByteArray toGeoMessage() const;

private:
  QSharedDataPointer<MessageData> d;
};

Q_DECLARE_METATYPE(Message::MessageAction)

#endif // MESSAGE_H
