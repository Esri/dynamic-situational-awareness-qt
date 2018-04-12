// Copyright 2018 ESRI
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

#include "AbstractMessageParser.h"
#include "CoTMessageParser.h"
#include "GeoMessageParser.h"
#include "SimulatedMessage.h"

#include <QFile>
#include <QXmlStreamReader>

AbstractMessageParser::AbstractMessageParser(const QString& filePath, QObject* parent) :
  QObject(parent),
  m_filePath(filePath)
{
}

AbstractMessageParser::~AbstractMessageParser()
{
}

AbstractMessageParser* AbstractMessageParser::createMessageParser(const QString& filePath, QObject* parent)
{
  QFile file(filePath);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    return nullptr;
  }

  QXmlStreamReader reader(&file);
  if (!reader.readNextStartElement() && !reader.isStartElement())
    return nullptr;

  const QString elementName = reader.name().toString();
  reader.clear();
  file.close();

  if (elementName.compare(SimulatedMessage::COT_ROOT_ELEMENT_NAME, Qt::CaseInsensitive) == 0 ||
      elementName.compare(SimulatedMessage::COT_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
  {
    return new CoTMessageParser(filePath, parent);
  }
  else if (elementName.compare(SimulatedMessage::GEOMESSAGE_ROOT_ELEMENT_NAME, Qt::CaseInsensitive) == 0 ||
      elementName.compare(SimulatedMessage::GEOMESSAGE_ELEMENT_NAME, Qt::CaseInsensitive) == 0)
  {
    return new GeoMessageParser(filePath, parent);
  }

  return nullptr;
}

QString AbstractMessageParser::filePath() const
{
  return m_filePath;
}
