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

// PCH header
#include "pch.hpp"

// dsa app headers
#include "Message.h"
#include "MessageFeedConstants.h"

// C++ API headers
#include "Point.h"
#include "PolygonBuilder.h"
#include "PolylineBuilder.h"
#include "SpatialReference.h"

// Qt headers
#include <QDomDocument>
#include <QXmlStreamReader>
#include <QString>

namespace Dsa {

using namespace Esri::ArcGISRuntime;

/*!
  \class Dsa::Message
  \inmodule Dsa
  \brief A message shared between applications.
 */

/*!
  \brief Default constructor.
 */
Message::Message() :
  d(new MessageData())
{
}

/*!
  \brief Constructor accepting a \a messageAction and a \a geometry.
 */
Message::Message(MessageAction messageAction, const Geometry& geometry) :
  d(new MessageData(messageAction, geometry))
{
}

/*!
  \brief Copy constructor.
 */
Message::Message(const Message& other) :
  d(other.d)
{
}

/*!
  \brief Move constructor.
 */
Message::Message(Message&& other) :
  d(std::move(other.d))
{
}

/*!
  \brief Destructor.
 */
Message::~Message()
{
}

/*!
  \brief Assignment operator.
 */
Message& Message::operator=(const Message& other)
{
  if (d != other.d)
  {
    d = other.d;
  }

  return *this;
}

/*!
  \brief Move operator.
 */
Message& Message::operator=(Message&& other)
{
  if (d != other.d)
  {
    d = std::move(other.d);
  }

  return *this;
}

/*!
  \brief Comparison operator.
 */
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

/*!
  \brief Static method to create a message from a QByteArray \a message.

  Demetermines if the provided bytes contain a CoT event
  or a GeoMessage and then forwards to the appropriate factory
 */
Message Message::create(const QByteArray& message)
{
  QDomDocument doc;
  if (!doc.setContent(message))
    return Message();

  // check root element name
  QDomElement parentElement = doc.firstChildElement(MessageFeeds::Parsing::CoT::ELEMENT_ROOT);
  if (!parentElement.isNull())
  {
    return createFromCoTMessage(message);
  }

  parentElement = doc.firstChildElement(MessageFeeds::Parsing::GeoMessage::ELEMENT_ROOT);
  if (!parentElement.isNull())
  {
    return createFromGeoMessage(message);
  }

  // fall back to individual element name
  parentElement = doc.firstChildElement(MessageFeeds::Parsing::CoT::ELEMENT_MSG);
  if (!parentElement.isNull())
  {
    return createFromCoTMessage(message);
  }

  parentElement = doc.firstChildElement(MessageFeeds::Parsing::GeoMessage::ELEMENT_MSG);
  if (!parentElement.isNull())
  {
    return createFromGeoMessage(message);
  }

  return Message();
}

/*!
  \brief Static method to create from a Cot (Cursor on Target) QByteArray \a message.
 */
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
      if (QString::compare(reader.name(), MessageFeeds::Parsing::CoT::ELEMENT_MSG, Qt::CaseInsensitive) == 0)
      {
        inCoTMessageElement = true;

        const auto attrs = reader.attributes();
        const auto type = attrs.value(MessageFeeds::Fields::CoT::TYPE).toString();
        // convert the CoT type to a sidc symbol code
        const auto sidc = cotTypeToSidc(type);
        if (sidc.isEmpty())
          return Message();

        // CoT is always an update action
        cotMessage.d->messageAction = MessageAction::Update;

        // CoT message type
        cotMessage.d->messageType = MessageFeeds::Types::CURSOR_ON_TARGET;
        attributes.insert(MessageFeeds::Fields::CoT::TYPE, type);

        // store the sidc symbol id code as an attribute of
        // the Message as well as the symbol Id variable
        cotMessage.d->symbolId = sidc;
        attributes.insert(MessageFeeds::Fields::Common::SIDC, sidc);

        // assign the unique message id
        cotMessage.d->messageId = attrs.value(MessageFeeds::Fields::CoT::UID).toString();
        attributes.insert(MessageFeeds::Fields::CoT::UID, cotMessage.d->messageId);

        // Add other CoT attributes
        const auto time = attrs.value(MessageFeeds::Fields::CoT::TIME).toString();
        const auto start = attrs.value(MessageFeeds::Fields::CoT::START).toString();
        const auto stale = attrs.value(MessageFeeds::Fields::CoT::STALE).toString();
        const auto how = attrs.value(MessageFeeds::Fields::CoT::HOW).toString();
        attributes.insert(MessageFeeds::Fields::CoT::HOW, how);
        attributes.insert(MessageFeeds::Fields::CoT::TIME, time);
        attributes.insert(MessageFeeds::Fields::CoT::START, start);
        attributes.insert(MessageFeeds::Fields::CoT::STALE, stale);
      }

      // before reading other element tags, make sure we are parsing a CoT element
      if (!inCoTMessageElement)
      {
        reader.readNext();
        continue;
      }

      if (QString::compare(reader.name(), MessageFeeds::Parsing::CoT::POINT, Qt::CaseInsensitive) == 0)
      {
        // parse the CoT point to populate the Message's geometry
        auto attrs = reader.attributes();
        bool lonOk = false;
        bool latOk = false;
        const auto lon = attrs.value(MessageFeeds::Fields::CoT::LON).toDouble(&lonOk);
        const auto lat = attrs.value(MessageFeeds::Fields::CoT::LAT).toDouble(&latOk);
        if (!lonOk || !latOk)
          return Message();

        const auto hae = attrs.value(MessageFeeds::Fields::CoT::HAE).toDouble();
        attributes.insert(MessageFeeds::Fields::CoT::LON, lon);
        attributes.insert(MessageFeeds::Fields::CoT::LAT, lat);
        attributes.insert(MessageFeeds::Fields::CoT::HAE, hae);

        cotMessage.d->geometry = Point(lon, lat, hae, SpatialReference::wgs84());
      }
    }
    else if (reader.isEndElement())
    {
      if (QString::compare(reader.name(), MessageFeeds::Parsing::CoT::ELEMENT_MSG, Qt::CaseInsensitive) == 0)
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

/*!
  \brief Static method to create from a GeoMessage QByteArray \a message.
 */
Message Message::createFromGeoMessage(const QByteArray& message)
{
  // parse GeoMessage XML bytes and build up a Message object from the
  // supplied information
  Message geoMessage;
  QVariantMap attributes;
  QString wkidText;
  QString controlPointsText;
  QString environmentText;

  bool inGeoMessageElement = false;

  QXmlStreamReader reader(message);

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // GeoMessage
      if (QString::compare(reader.name(), MessageFeeds::Parsing::GeoMessage::ELEMENT_MSG, Qt::CaseInsensitive) == 0)
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

      if (QString::compare(reader.name(), MessageFeeds::Fields::GeoMessage::TYPE, Qt::CaseInsensitive) == 0)
      {
        geoMessage.d->messageType = reader.readElementText();
        attributes.insert(MessageFeeds::Fields::GeoMessage::TYPE, geoMessage.d->messageType);
      }
      else if (QString::compare(reader.name(), MessageFeeds::Fields::GeoMessage::ACTION, Qt::CaseInsensitive) == 0)
      {
        const QString actionText = reader.readElementText();
        geoMessage.d->messageAction = toMessageAction(actionText);
        attributes.insert(MessageFeeds::Fields::GeoMessage::ACTION, actionText);
      }
      else if (QString::compare(reader.name(), MessageFeeds::Fields::GeoMessage::ID, Qt::CaseInsensitive) == 0)
      {
        geoMessage.d->messageId = reader.readElementText();
        attributes.insert(MessageFeeds::Fields::GeoMessage::ID, geoMessage.d->messageId);
      }
      else if (QString::compare(reader.name(), MessageFeeds::Fields::GeoMessage::WKID, Qt::CaseInsensitive) == 0)
      {
        wkidText = reader.readElementText();
        attributes.insert(MessageFeeds::Fields::GeoMessage::WKID, wkidText);
      }
      else if (QString::compare(reader.name(), MessageFeeds::Fields::GeoMessage::SIC, Qt::CaseInsensitive) == 0)
      {
        geoMessage.d->symbolId = reader.readElementText();
        attributes.insert(MessageFeeds::Fields::GeoMessage::SIC, geoMessage.d->symbolId);
        attributes.insert(MessageFeeds::Fields::Common::SIDC, geoMessage.d->symbolId);
      }
      else if (QString::compare(reader.name(), MessageFeeds::Fields::GeoMessage::CONTROL_POINTS, Qt::CaseInsensitive) == 0)
      {
        controlPointsText = reader.readElementText();
        attributes.insert(MessageFeeds::Fields::GeoMessage::CONTROL_POINTS, controlPointsText);
      }
      else if (QString::compare(reader.name(), MessageFeeds::Fields::GeoMessage::ENVIRONMENT, Qt::CaseInsensitive) == 0)
      {
        environmentText = reader.readElementText();
        attributes.insert(MessageFeeds::Fields::GeoMessage::ENVIRONMENT, environmentText);
      }
      else
      {
        attributes.insert(reader.name().toString(), reader.readElementText());
      }
    }
    else if (reader.isEndElement())
    {
      if (QString::compare(reader.name(), MessageFeeds::Parsing::GeoMessage::ELEMENT_MSG, Qt::CaseInsensitive) == 0)
      {
        inGeoMessageElement = false;
      }
    }

    reader.readNext();
  }

  if (!environmentText.isEmpty())
  {
    geoMessage.d->messageType += QString("_%1").arg(environmentText);
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

/*!
  \brief Static method to convert a CoT type string \a cotType to a SIDC string.
 */
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

/*!
  \brief Static method to convert an \a action string to a MessageAction enum value.
 */
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

/*!
  \brief Static method to convert from a MessageAction enum value (\a action) to a string.
 */
QString Message::fromMessageAction(MessageAction action)
{
  switch (action)
  {
  case MessageAction::Remove:
    return QStringLiteral("remove");
  case MessageAction::Select:
    return QStringLiteral("select");
  case MessageAction::Unselect:
    return QStringLiteral("un-select");
  case MessageAction::Update:
    return QStringLiteral("update");
  default:
    break;
  }

  return QString();
}

/*!
  \brief Returns whether the message is empty.
 */
bool Message::isEmpty() const
{
  return d->attributes.isEmpty() && d->geometry.isEmpty() &&
      d->messageId.isEmpty() && d->messageName.isEmpty() &&
      d->messageType.isEmpty() && d->symbolId.isEmpty() &&
      d->messageAction == MessageAction::Unknown;
}

/*!
  \brief Returns the current message action.
 */
Message::MessageAction Message::messageAction() const
{
  return d->messageAction;
}

/*!
  \brief Sets the current message action to \a messageAction.
 */
void Message::setMessageAction(MessageAction messageAction)
{
  d->messageAction = messageAction;
}

/*!
  \brief Returns the current message attributes.
 */
QVariantMap Message::attributes() const
{
  return d->attributes;
}

/*!
  \brief Sets the current message attributes to \a attributes.
 */
void Message::setAttributes(const QVariantMap& attributes)
{
  d->attributes = attributes;
}

/*!
  \brief Returns the current message geometry.
 */
Geometry Message::geometry() const
{
  return d->geometry;
}

/*!
  \brief Sets the current message geometry to \a geometry.
 */
void Message::setGeometry(const Geometry& geometry)
{
  d->geometry = geometry;
}

/*!
  \brief Returns the current message ID.
 */
QString Message::messageId() const
{
  return d->messageId;
}

/*!
  \brief Sets the current message ID to \a messageId.
 */
void Message::setMessageId(const QString& messageId)
{
  d->messageId = messageId;
}

/*!
  \brief Returns the current message name.
 */
QString Message::messageName() const
{
  return d->messageName;
}

/*!
  \brief Sets the current message name to \a messageName.
 */
void Message::setMessageName(const QString& messageName)
{
  d->messageName = messageName;
}

/*!
  \brief Returns the current message type.
 */
QString Message::messageType() const
{
  return d->messageType;
}

/*!
  \brief Sets the current message type to \a messageType.
 */
void Message::setMessageType(const QString& messageType)
{
  d->messageType = messageType;
}

/*!
  \brief Returns the current symbol ID.
 */
QString Message::symbolId() const
{
  return d->symbolId;
}

/*!
  \brief Sets the current symbol ID to \a symbolId.
 */
void Message::setSymbolId(const QString& symbolId)
{
  d->symbolId = symbolId;
}

/*!
  \brief Returns the current message as QByteArray in the GeoMessage format.
 */
QByteArray Message::toGeoMessage() const
{
  QByteArray message;
  QXmlStreamWriter streamWriter(&message);

  streamWriter.writeStartElement(MessageFeeds::Parsing::GeoMessage::ELEMENT_MSG);

  streamWriter.writeStartElement(MessageFeeds::Fields::GeoMessage::TYPE);
  streamWriter.writeCharacters(messageType());
  streamWriter.writeEndElement();

  streamWriter.writeStartElement(MessageFeeds::Fields::GeoMessage::ACTION);
  streamWriter.writeCharacters(fromMessageAction(messageAction()));
  streamWriter.writeEndElement();

  streamWriter.writeStartElement(MessageFeeds::Fields::GeoMessage::ID);
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

  streamWriter.writeStartElement(MessageFeeds::Fields::GeoMessage::CONTROL_POINTS);
  streamWriter.writeCharacters(controlPoints);
  streamWriter.writeEndElement();

  streamWriter.writeStartElement(MessageFeeds::Fields::GeoMessage::WKID);
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

/*!
  \internal
 */
MessageData::MessageData()
{
}

/*!
  \internal
 */
MessageData::MessageData(Message::MessageAction messageAction, const Geometry& geometry) :
  messageAction(messageAction),
  geometry(geometry)
{
}

/*!
  \internal
 */
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

/*!
  \internal
 */
MessageData::~MessageData()
{
}

} // Dsa
