
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
