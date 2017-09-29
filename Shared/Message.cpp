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

static const QString s_cotElementName{QStringLiteral("event")};
static const QString s_cotTypeName{QStringLiteral("type")};
static const QString s_cotUidName{QStringLiteral("uid")};
static const QString s_cotPointName{QStringLiteral("point")};
static const QString s_cotPointLatName{QStringLiteral("lat")};
static const QString s_cotPointLonName{QStringLiteral("lon")};
static const QString s_cotPointHaeName{QStringLiteral("hae")};
static const QString s_sidcName{QStringLiteral("sidc")};

using namespace Esri::ArcGISRuntime;

Message::Message() :
  d(new MessageData())
{
}

Message::Message(MessageAction messageAction, const Geometry& geometry) :
  d(new MessageData(messageAction, geometry))
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

bool Message::operator==(const Message& other) const
{
  return messageAction() == other.messageAction() &&
      attributes() == other.attributes() &&
      geometry() == other.geometry() &&
      messageId() == other.messageId() &&
      messageName() == other.messageName() &&
      messageType() == other.messageType() &&
      symbolId() == other.symbolId();
}

Message Message::createFromCoTMessage(const QByteArray& message)
{
  // parse CoT XML bytes and build up a Message object from the
  // supplied information
  Message cotMessage;
  QVariantMap cotAttributes;

  QXmlStreamReader reader(message);

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // CoT event
      if (QStringRef::compare(reader.name(), s_cotElementName, Qt::CaseInsensitive) == 0)
      {
        const auto attrs = reader.attributes();
        const auto type = attrs.value(s_cotTypeName).toString();
        // convert the CoT type to a sidc symbol code
        const auto sidc = cotTypeToSidc(type);
        if (sidc.isEmpty())
          return Message();

        // CoT is always an update action
        cotMessage.d->messageAction = MessageAction::Update;

        // store the sidc symbol id code as an attribute of
        // the Message as well as the symbol Id variable
        cotAttributes.insert(s_sidcName, sidc);
        cotMessage.d->symbolId = sidc;

        // assign the unique message id
        cotMessage.d->messageId = attrs.value(s_cotUidName).toString();
      }
      else if (QStringRef::compare(reader.name(), s_cotPointName, Qt::CaseInsensitive) == 0)
      {
        // parse the CoT point to populate the Message's geometry
        auto attrs = reader.attributes();
        bool lonOk = false;
        bool latOk = false;
        const auto lon = attrs.value(s_cotPointLonName).toDouble(&lonOk);
        const auto lat = attrs.value(s_cotPointLatName).toDouble(&latOk);
        if (!lonOk || !latOk)
          return Message();

        const auto hae = attrs.value(s_cotPointHaeName).toDouble();

        cotMessage.d->geometry = Point(lon, lat, hae, SpatialReference::wgs84());
      }
    }

    reader.readNext();
  }

  // assign the Message attributes
  cotMessage.d->attributes = cotAttributes;

  return cotMessage;
}

QString Message::cotTypeToSidc(const QString& cotType)
{
  // converts a CoT type to a sidc symbol id code
  // For example: CoT type: a-f-S-C-A to sidc: SFSPCA---------
  QString retVal;

  // recognized affiliation types for converted between CoT type
  // and sidc symbols
  const QString recognizedAffiliations(QStringLiteral("fhupansjku"));

  // recognized battle space types for converting between CoT type
  // and sidc symbols
  const QString recognizedBattleSpaces(QStringLiteral("PAGSUF"));

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
      d->messageType == MessageType::Unknown && d->symbolId.isEmpty() &&
      d->messageAction == MessageAction::Unknown;
}

Message::MessageAction Message::messageAction() const
{
  return d->messageAction;
}

void Message::setMessageAction(MessageAction messageAction)
{
  d->messageAction = messageAction;
}

QVariantMap Message::attributes() const
{
  return d->attributes;
}

void Message::setAttributes(const QVariantMap& attributes)
{
  d->attributes = attributes;
}

Geometry Message::geometry() const
{
  return d->geometry;
}

void Message::setGeometry(const Geometry& geometry)
{
  d->geometry = geometry;
}

QString Message::messageId() const
{
  return d->messageId;
}

void Message::setMessageId(const QString& messageId)
{
  d->messageId = messageId;
}

QString Message::messageName() const
{
  return d->messageName;
}

void Message::setMessageName(const QString& messageName)
{
  d->messageName = messageName;
}

Message::MessageType Message::messageType() const
{
  return d->messageType;
}

void Message::setMessageType(MessageType messageType)
{
  d->messageType = messageType;
}

QString Message::symbolId() const
{
  return d->symbolId;
}

void Message::setSymbolId(const QString& symbolId)
{
  d->symbolId = symbolId;
}

MessageData::MessageData()
{
}

MessageData::MessageData(Message::MessageAction messageAction, const Geometry& geometry) :
  messageAction(messageAction),
  geometry(geometry)
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
