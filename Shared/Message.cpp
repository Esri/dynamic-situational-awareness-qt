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

#include "Message.h"
#include "Point.h"
#include "Message_p.h"

#include <QXmlStreamReader>

using namespace Esri::ArcGISRuntime;

Message::Message() :
  d(new MessageData())
{
}

Message::Message(const Message& other) :
  d(other.d)
{
}

Message::Message(Message&& other) :
  d(std::move(other.d))
{
}

Message::~Message()
{
}

Message& Message::operator=(const Message& other)
{
  if (d != other.d)
  {
    d = other.d;
  }

  return *this;
}

Message& Message::operator=(Message&& other)
{
  if (d != other.d)
  {
    d = std::move(other.d);
  }

  return *this;
}

Message Message::createFromCoTMessage(const QByteArray& message)
{
  Message cotMessage;
  QVariantMap cotAttributes;

  QXmlStreamReader reader(message);

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // CoT event
      if (QStringRef::compare(reader.name(), "event") == 0)
      {
        const auto attrs = reader.attributes();
        const auto type = attrs.value("type").toString();
        const auto sidc = cotTypeToSidc(type);
        if (sidc.isEmpty())
          return Message();

        cotMessage.d->messageAction = MessageAction::Update;

        cotAttributes.insert("sidc", sidc);
        cotMessage.d->symbolId = sidc;

        cotMessage.d->messageId = attrs.value("uid").toString();
      }
      else if (QStringRef::compare(reader.name(), "point") == 0)
      {
        auto attrs = reader.attributes();
        bool lonOk = false;
        bool latOk = false;
        const auto lon = attrs.value("lon").toDouble(&lonOk);
        const auto lat = attrs.value("lat").toDouble(&latOk);
        if (!lonOk || !latOk)
          return Message();

        const auto hae = attrs.value("hae").toDouble();

        cotMessage.d->geometry = Point(lon, lat, hae, SpatialReference::wgs84());
      }
    }

    reader.readNext();
  }

  cotMessage.d->attributes = cotAttributes;

  return cotMessage;
}

QString Message::cotTypeToSidc(const QString& cotType)
{
  QString retVal;

  const QString recognizedAffiliations("fhupansjku");
  const QString recognizedBattleSpaces("PAGSUF");

  if (cotType.mid(0, 1) != "a")
    return QString();

  // Must be of the atom type or it is not supported
  retVal += "S";

  // Convert affiliation
  if (!recognizedAffiliations.contains(cotType.mid(2, 1)))
    return QString();

  retVal += cotType.mid(2, 1).toUpper();

  // Convert battle space dimension
  if (!recognizedBattleSpaces.contains(cotType.mid(4, 1)))
    return QString();

  retVal += cotType.mid(4, 1);

  // All CoT types assumed Present (as opposed to
  // anticipated/planned)
  retVal += "P";

  // All remaining capital letters in the string are 1:1
  // equivalents of CoT codes (although not all 2525b codes
  // are used in CoT).
  QString remainingChars = cotType.mid(6, cotType.length() - 6);
  for (int i=0; i<remainingChars.length(); i=i+2)
  {
    retVal += remainingChars[i];
  }

  while (retVal.length() < 15)
  {
    retVal += "-";
  }

  return retVal;
}

bool Message::isEmpty() const
{
  return d->attributes.isEmpty() && d->geometry.isEmpty() &&
      d->messageId.isEmpty() && d->messageName.isEmpty() &&
      d->messageType.isEmpty() && d->symbolId.isEmpty() &&
      d->messageAction == MessageAction::Unknown;
}

Message::MessageAction Message::messageAction() const
{
  return d->messageAction;
}

QVariantMap Message::attributes() const
{
  return d->attributes;
}

Geometry Message::geometry() const
{
  return d->geometry;
}

QString Message::messageId() const
{
  return d->messageId;
}

QString Message::messageName() const
{
  return d->messageName;
}

QString Message::messageType() const
{
  return d->messageType;
}

QString Message::symbolId() const
{
  return d->symbolId;
}

MessageData::MessageData()
{
}

MessageData::MessageData(const MessageData& other) :
  QSharedData(other),
  messageAction(other.messageAction),
  attributes(other.attributes),
  geometry(other.geometry),
  messageId(other.messageId),
  messageName(other.messageName),
  messageType(other.messageType),
  symbolId(other.symbolId)
{
}

MessageData::~MessageData()
{
}
