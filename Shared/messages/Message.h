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

#ifndef MESSAGE_H
#define MESSAGE_H

// Qt headers
#include <QSharedData>
#include <QVariantMap>

// C++ API headers
#include "Geometry.h"

namespace Dsa {

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

class MessageData : public QSharedData
{
public:
  MessageData();
  MessageData(Message::MessageAction messageAction, const Esri::ArcGISRuntime::Geometry& geometry);
  MessageData(const MessageData& other);
  ~MessageData();

  Message::MessageAction messageAction = Message::MessageAction::Unknown;
  QVariantMap attributes;
  Esri::ArcGISRuntime::Geometry geometry;
  QString messageId;
  QString messageName;
  QString messageType;
  QString symbolId;
};

} // Dsa

Q_DECLARE_METATYPE(Dsa::Message::MessageAction)

#endif // MESSAGE_H
