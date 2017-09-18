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

#ifndef MESSAGE_P_H
#define MESSAGE_P_H

#include "Message.h"

class MessageData : public QSharedData
{
public:
  MessageData();
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

#endif // MESSAGE_P_H
