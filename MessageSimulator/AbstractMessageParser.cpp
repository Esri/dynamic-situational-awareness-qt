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

#include "AbstractMessageParser.h"

AbstractMessageParser::AbstractMessageParser(const QString& filePath, QObject* parent) :
  QObject(parent),
  m_filePath(filePath)
{
}

AbstractMessageParser::~AbstractMessageParser()
{
}

QString AbstractMessageParser::filePath() const
{
  return m_filePath;
}
