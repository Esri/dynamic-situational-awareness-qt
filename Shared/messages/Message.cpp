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
#include "PolygonBuilder.h"
#include "PolylineBuilder.h"

#include <QXmlStreamReader>
#include <QDomDocument>

const QString Message::COT_ROOT_ELEMENT_NAME{QStringLiteral("events")};
const QString Message::COT_ELEMENT_NAME{QStringLiteral("event")};
const QString Message::COT_TYPE_NAME{QStringLiteral("type")};
const QString Message::COT_UID_NAME{QStringLiteral("uid")};
const QString Message::COT_POINT_NAME{QStringLiteral("point")};
const QString Message::COT_POINT_LAT_NAME{QStringLiteral("lat")};
const QString Message::COT_POINT_LON_NAME{QStringLiteral("lon")};
const QString Message::COT_POINT_HAE_NAME{QStringLiteral("hae")};

const QString Message::GEOMESSAGE_ROOT_ELEMENT_NAME{QStringLiteral("geomessages")};
const QString Message::GEOMESSAGE_ELEMENT_NAME{QStringLiteral("geomessage")};
const QString Message::GEOMESSAGE_TYPE_NAME{QStringLiteral("_type")};
const QString Message::GEOMESSAGE_ACTION_NAME{QStringLiteral("_action")};
const QString Message::GEOMESSAGE_ID_NAME{QStringLiteral("_id")};
const QString Message::GEOMESSAGE_WKID_NAME{QStringLiteral("_wkid")};
const QString Message::GEOMESSAGE_SIC_NAME{QStringLiteral("sic")};
const QString Message::GEOMESSAGE_CONTROL_POINTS_NAME{QStringLiteral("_control_points")};
const QString Message::GEOMESSAGE_UNIQUE_DESIGNATION_NAME{QStringLiteral("uniquedesignation")};
const QString Message::GEOMESSAGE_STATUS_911_NAME{QStringLiteral("status911")};

const QString Message::SIDC_NAME{QStringLiteral("sidc")};

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

// Demetermines if the provided bytes contain a CoT event
// or a GeoMessage and then forwards to the appropriate factory
Message Message::create(const QByteArray& message)
{
  QDomDocument doc;
  if (!doc.setContent(message))
    return Message();

  // check root element name
  QDomElement parentElement = doc.firstChildElement(COT_ROOT_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromCoTMessage(message);
  }

  parentElement = doc.firstChildElement(GEOMESSAGE_ROOT_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromGeoMessage(message);
  }

  // fall back to individual element name
  parentElement = doc.firstChildElement(COT_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromCoTMessage(message);
  }

  parentElement = doc.firstChildElement(GEOMESSAGE_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromGeoMessage(message);
  }

  return Message();
}

Message Message::createFromCoTMessage(const QByteArray& message)
{
  // parse CoT XML bytes and build up a Message object from the
  // supplied information
  Message cotMessage;
  QVariantMap attributes;

  bool inCoTMessageElement = false;

  QXmlStreamReader reader(message);

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // CoT event
      if (QStringRef::compare(reader.name(), COT_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
      {
        inCoTMessageElement = true;

        const auto attrs = reader.attributes();
        const auto type = attrs.value(COT_TYPE_NAME).toString();
        // convert the CoT type to a sidc symbol code
        const auto sidc = cotTypeToSidc(type);
        if (sidc.isEmpty())
          return Message();

        // CoT is always an update action
        cotMessage.d->messageAction = MessageAction::Update;

        // CoT message type
        cotMessage.d->messageType = QStringLiteral("cot");

        // store the sidc symbol id code as an attribute of
        // the Message as well as the symbol Id variable
        attributes.insert(SIDC_NAME, sidc);
        cotMessage.d->symbolId = sidc;

        // assign the unique message id
        cotMessage.d->messageId = attrs.value(COT_UID_NAME).toString();
      }

      // before reading other element tags, make sure we are parsing a CoT element
      if (!inCoTMessageElement)
      {
        reader.readNext();
        continue;
      }

      if (QStringRef::compare(reader.name(), COT_POINT_NAME, Qt::CaseInsensitive) == 0)
      {
        // parse the CoT point to populate the Message's geometry
        auto attrs = reader.attributes();
        bool lonOk = false;
        bool latOk = false;
        const auto lon = attrs.value(COT_POINT_LON_NAME).toDouble(&lonOk);
        const auto lat = attrs.value(COT_POINT_LAT_NAME).toDouble(&latOk);
        if (!lonOk || !latOk)
          return Message();

        const auto hae = attrs.value(COT_POINT_HAE_NAME).toDouble();

        cotMessage.d->geometry = Point(lon, lat, hae, SpatialReference::wgs84());
      }
    }
    else if (reader.isEndElement())
    {
      if (QStringRef::compare(reader.name(), COT_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
      {
        inCoTMessageElement = false;
      }
    }

    reader.readNext();
  }

  // assign the Message attributes
  cotMessage.d->attributes = attributes;

  return cotMessage;
}

Message Message::createFromGeoMessage(const QByteArray& message)
{
  // parse GeoMessage XML bytes and build up a Message object from the
  // supplied information
  Message geoMessage;
  QVariantMap attributes;
  QString wkidText;
  QString controlPointsText;

  bool inGeoMessageElement = false;

  QXmlStreamReader reader(message);

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // GeoMessage
      if (QStringRef::compare(reader.name(), GEOMESSAGE_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
      {
        inGeoMessageElement = true;
        reader.readNext();
        continue;
      }

      // before reading other element tags, make sure we are parsing a GeoMessage element
      if (!inGeoMessageElement)
      {
        reader.readNext();
        continue;
      }

      if (QStringRef::compare(reader.name(), GEOMESSAGE_TYPE_NAME, Qt::CaseInsensitive) == 0)
      {
        geoMessage.d->messageType = reader.readElementText();
      }
      else if (QStringRef::compare(reader.name(), GEOMESSAGE_ACTION_NAME, Qt::CaseInsensitive) == 0)
      {
        const QString actionText = reader.readElementText();
        geoMessage.d->messageAction = toMessageAction(actionText);
      }
      else if (QStringRef::compare(reader.name(), GEOMESSAGE_ID_NAME, Qt::CaseInsensitive) == 0)
      {
        geoMessage.d->messageId = reader.readElementText();
      }
      else if (QStringRef::compare(reader.name(), GEOMESSAGE_WKID_NAME, Qt::CaseInsensitive) == 0)
      {
        wkidText = reader.readElementText();
      }
      else if (QStringRef::compare(reader.name(), GEOMESSAGE_SIC_NAME, Qt::CaseInsensitive) == 0)
      {
        const auto sidc = reader.readElementText();
        attributes.insert(SIDC_NAME, sidc);
        geoMessage.d->symbolId = sidc;
      }
      else if (QStringRef::compare(reader.name(), GEOMESSAGE_CONTROL_POINTS_NAME, Qt::CaseInsensitive) == 0)
      {
        controlPointsText = reader.readElementText();
      }
      else
      {
        attributes.insert(reader.name().toString(), reader.readElementText());
      }
    }
    else if (reader.isEndElement())
    {
      if (QStringRef::compare(reader.name(), GEOMESSAGE_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
      {
        inGeoMessageElement = false;
      }
    }

    reader.readNext();
  }

  if (!controlPointsText.isEmpty())
  {
    const SpatialReference sr = wkidText.isEmpty() ? SpatialReference::wgs84() : SpatialReference(wkidText.toInt());

    const QStringList controlPoints = controlPointsText.split(";");
    bool isMultipart = controlPoints.size() > 1;

    if (isMultipart)
    {
      // if first and last points are equal, then this is a closed polygon geometry
      bool isPolygon = controlPoints.first() == controlPoints.last();
      QObject localParent;
      MultipartBuilder* multiPartBuilder = nullptr;
      if (isPolygon)
        multiPartBuilder = new PolygonBuilder(sr, &localParent);
      else
        multiPartBuilder = new PolylineBuilder(sr, &localParent);

      // multipart geometry
      for (const QString& controlPoint : controlPoints)
      {
        const auto controlPointValues = controlPoint.split(",");
        if (controlPointValues.size() == 2)
        {
          // 2D point
          multiPartBuilder->addPoint(controlPointValues[0].toDouble(), controlPointValues[1].toDouble());
        }
        else
        {
          // 3D point
          multiPartBuilder->addPoint(controlPointValues[0].toDouble(), controlPointValues[1].toDouble(), controlPointValues[2].toDouble());
        }
      }

      geoMessage.d->geometry = multiPartBuilder->toGeometry();
    }
    else if (controlPoints.size() == 1)
    {
      // single point geometry
      const QStringList controlPointValues = controlPoints[0].split(",");
      if (controlPointValues.size() == 2)
      {
        // 2D point
        geoMessage.d->geometry = Point(controlPointValues[0].toDouble(), controlPointValues[1].toDouble(), sr);
      }
      else
      {
        // 3D point
        geoMessage.d->geometry = Point(controlPointValues[0].toDouble(), controlPointValues[1].toDouble(), controlPointValues[2].toDouble(), sr);
      }
    }
  }

  // assign the Message attributes
  geoMessage.d->attributes = attributes;

  return geoMessage;
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

Message::MessageAction Message::toMessageAction(const QString& action)
{
  if (action.compare("remove", Qt::CaseInsensitive) == 0)
    return MessageAction::Remove;
  else if (action.compare("select", Qt::CaseInsensitive) == 0)
    return MessageAction::Select;
  else if (action.compare("un-select", Qt::CaseInsensitive) == 0)
    return MessageAction::Unselect;
  else if (action.compare("update", Qt::CaseInsensitive) == 0)
    return MessageAction::Update;

  return MessageAction::Unknown;
}

QString Message::fromMessageAction(MessageAction action)
{
  switch (action)
  {
  case MessageAction::Remove:
    return "remove";
  case MessageAction::Select:
    return "select";
  case MessageAction::Unselect:
    return "un-select";
  case MessageAction::Update:
    return "update";
  default:
    break;
  }

  return QString();
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

QString Message::messageType() const
{
  return d->messageType;
}

void Message::setMessageType(const QString& messageType)
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

QByteArray Message::toGeoMessage() const
{
  QByteArray message;
  QXmlStreamWriter streamWriter(&message);

  streamWriter.writeStartElement(GEOMESSAGE_ELEMENT_NAME);

  streamWriter.writeStartElement(GEOMESSAGE_TYPE_NAME);
  streamWriter.writeCharacters(messageType());
  streamWriter.writeEndElement();

  streamWriter.writeStartElement(GEOMESSAGE_ACTION_NAME);
  streamWriter.writeCharacters(fromMessageAction(messageAction()));
  streamWriter.writeEndElement();

  streamWriter.writeStartElement(GEOMESSAGE_ID_NAME);
  streamWriter.writeCharacters(messageId());
  streamWriter.writeEndElement();

  QString controlPoints;
  switch (geometry().geometryType())
  {
  case GeometryType::Point:
  {
    Point pt = geometry_cast<Point>(geometry());
    controlPoints = QString("%1,%2").arg(QString::number(pt.x(), 'g', 9), QString::number(pt.y(), 'g', 9));
  }
  default:
    break;
  }

  streamWriter.writeStartElement(GEOMESSAGE_CONTROL_POINTS_NAME);
  streamWriter.writeCharacters(controlPoints);
  streamWriter.writeEndElement();

  streamWriter.writeStartElement(GEOMESSAGE_WKID_NAME);
  streamWriter.writeCharacters(QString::number(geometry().spatialReference().wkid()));
  streamWriter.writeEndElement();

  const auto attribs = attributes();
  for (QVariantMap::const_iterator iter = attribs.constBegin(); iter != attribs.constEnd(); ++iter)
  {
    const auto key = iter.key();
    if (key.startsWith("_")) // attributes which start with "_" are stored in member variables
      continue;

    streamWriter.writeStartElement(key);
    streamWriter.writeCharacters(iter.value().toString());
    streamWriter.writeEndElement();
  }

  streamWriter.writeEndElement(); // end geomessage

  return message;
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
