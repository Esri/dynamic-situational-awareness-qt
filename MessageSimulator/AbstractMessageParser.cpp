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
