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

#include "CoTMessageParser.h"

static const QString s_cotElementName{"event"};

CoTMessageParser::CoTMessageParser(const QString& filePath, QObject* parent) :
  AbstractMessageParser(filePath, parent)
{
}

CoTMessageParser::~CoTMessageParser()
{
  // close and clear device
  reset();
}

QByteArray CoTMessageParser::nextMessage()
{
  if (!m_isParsing)
  {
    // if not parsing message file yet,
    // open the file for read access
    m_device.setFileName(filePath());
    if (!m_device.open(QFile::ReadOnly | QFile::Text))
    {
      emit errorOccurred(tr("Could not open ") + filePath() + tr(" for reading"));
      return QByteArray();
    }

    m_reader.setDevice(&m_device);

    m_isParsing = true;
  }

  if (atEnd())
  {
    emit errorOccurred(tr("Finished parsing messages to end of file"));
    return QByteArray();
  }

  QByteArray message;
  QXmlStreamWriter streamWriter(&message);

  bool readingMessage = false;

  // loop through XML stream until reaching the end
  while (!atEnd())
  {
    if (m_reader.isEndElement())
    {
      streamWriter.writeEndElement();
      if (m_reader.name().compare(s_cotElementName, Qt::CaseInsensitive) == 0)
      {
        // finished reading single CoT element
        readingMessage = false;
        m_reader.readNext();
        break;
      }
    }
    else if (m_reader.isStartElement())
    {
      const QString elementName = m_reader.name().toString();

      if (!readingMessage && elementName.compare(s_cotElementName, Qt::CaseInsensitive) != 0)
      {
        // begin reading single CoT element
        m_reader.readNext();
        continue;
      }

      readingMessage = true;

      // write element and attributes to the stream writer
      if (m_reader.prefix().toString().isEmpty())
      {
        streamWriter.writeStartElement(elementName);
      }
      else
      {
        streamWriter.writeStartElement(m_reader.prefix().toString()+ ":" + elementName);
      }

      if (!m_reader.attributes().isEmpty())
      {
        streamWriter.writeAttributes(m_reader.attributes());
      }
    }
    else if (m_reader.isCharacters())
    {
      // write element text to the stream writer
      streamWriter.writeCharacters(m_reader.text().toString());
    }

    m_reader.readNext();
  }

  return message;
}

void CoTMessageParser::reset()
{
  // close and reset device and parser
  if (m_device.isOpen())
    m_device.close();

  m_device.setFileName(QString());
  m_reader.clear();
  m_isParsing = false;
}

bool CoTMessageParser::atEnd() const
{
  return m_reader.atEnd();
}
