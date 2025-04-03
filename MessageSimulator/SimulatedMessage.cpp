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

#include "SimulatedMessage.h"
#include "AbstractMessageParser.h"

#include <QDomDocument>
#include <QXmlStreamReader>

const QString SimulatedMessage::COT_ROOT_ELEMENT_NAME{QStringLiteral("events")};
const QString SimulatedMessage::COT_ELEMENT_NAME{QStringLiteral("event")};
const QString SimulatedMessage::COT_TYPE_NAME{QStringLiteral("type")};
const QString SimulatedMessage::COT_UID_NAME{QStringLiteral("uid")};

const QString SimulatedMessage::GEOMESSAGE_ROOT_ELEMENT_NAME{QStringLiteral("geomessages")};
const QString SimulatedMessage::GEOMESSAGE_ELEMENT_NAME{QStringLiteral("geomessage")};
const QString SimulatedMessage::GEOMESSAGE_ID_NAME{QStringLiteral("_id")};
const QString SimulatedMessage::GEOMESSAGE_SIC_NAME{QStringLiteral("sic")};

SimulatedMessage::SimulatedMessage(QObject* parent) :
  QObject(parent)
{
}

SimulatedMessage::SimulatedMessage(MessageFormat messageFormat, const QString& messageId, const QString& messageAction, const QString& symbolId, QObject* parent) :
  QObject(parent),
  m_messageFormat(messageFormat),
  m_messageId(messageId),
  m_messageAction(messageAction),
  m_symbolId(symbolId)
{
}

SimulatedMessage::~SimulatedMessage()
{
}

SimulatedMessage* SimulatedMessage::create(const QByteArray& message, QObject* parent)
{
  QDomDocument doc;
  if (!doc.setContent(message))
    return nullptr;

  // check root element name
  QDomElement parentElement = doc.firstChildElement(COT_ROOT_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromCoTMessage(message, parent);
  }

  parentElement = doc.firstChildElement(GEOMESSAGE_ROOT_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromGeoMessage(message, parent);
  }

  // fall back to individual element name
  parentElement = doc.firstChildElement(COT_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromCoTMessage(message, parent);
  }

  parentElement = doc.firstChildElement(GEOMESSAGE_ELEMENT_NAME);
  if (!parentElement.isNull())
  {
    return createFromGeoMessage(message, parent);
  }

  return nullptr;
}

SimulatedMessage* SimulatedMessage::createFromCoTMessage(const QByteArray& message, QObject* parent)
{
  QXmlStreamReader reader(message);

  QString messageId;
  QString symbolId;

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // CoT event
      if (QString::compare(reader.name(), COT_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
      {
        const auto attrs = reader.attributes();
        symbolId = attrs.value(COT_TYPE_NAME).toString();
        messageId = attrs.value(COT_UID_NAME).toString();

        if (symbolId.isEmpty() || messageId.isEmpty())
          return nullptr;
      }
    }

    reader.readNext();
  }

  return new SimulatedMessage(MessageFormat::CoT, messageId, QStringLiteral("update"), symbolId, parent);
}

SimulatedMessage* SimulatedMessage::createFromGeoMessage(const QByteArray& message, QObject* parent)
{
  QXmlStreamReader reader(message);

  QString messageId;
  QString symbolId;

  bool inGeoMessageElement = false;

  while (!reader.atEnd() && !reader.hasError())
  {
    if (reader.isStartElement())
    {
      // GeoMessage
      if (QString::compare(reader.name(), GEOMESSAGE_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
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

      if (QString::compare(reader.name(), GEOMESSAGE_ID_NAME, Qt::CaseInsensitive) == 0)
      {
        messageId = reader.readElementText();
      }
      else if (QString::compare(reader.name(), GEOMESSAGE_SIC_NAME, Qt::CaseInsensitive) == 0)
      {
        const auto sidc = reader.readElementText();
        symbolId = sidc;
      }
    }
    else if (reader.isEndElement())
    {
      if (QString::compare(reader.name(), GEOMESSAGE_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
      {
        inGeoMessageElement = false;
      }
    }

    if (!messageId.isEmpty() && !symbolId.isEmpty())
    {
      return new SimulatedMessage(MessageFormat::GeoMessage, messageId, QStringLiteral("update"), symbolId, parent);
    }

    reader.readNext();
  }

  return nullptr;
}

SimulatedMessage::MessageFormat SimulatedMessage::messageFormat() const
{
  return m_messageFormat;
}

void SimulatedMessage::setMessageFormat(MessageFormat messageFormat)
{
  m_messageFormat = messageFormat;
}

QString SimulatedMessage::messageFormatString() const
{
  switch (m_messageFormat)
  {
  case MessageFormat::CoT:
    return QStringLiteral("CoT");
  case MessageFormat::GeoMessage:
    return QStringLiteral("GeoMessage");
  default:
    return QString();
  }

  return QString();
}

QString SimulatedMessage::messageId() const
{
  return m_messageId;
}

void SimulatedMessage::setMessageId(const QString& messageId)
{
  m_messageId = messageId;
}

QString SimulatedMessage::messageAction() const
{
  return m_messageAction;
}

void SimulatedMessage::setMessageAction(const QString& messageAction)
{
  m_messageAction = messageAction;
}

QString SimulatedMessage::symbolId() const
{
  return m_symbolId;
}

void SimulatedMessage::setSymbolId(const QString& symbolId)
{
  m_symbolId = symbolId;
}
