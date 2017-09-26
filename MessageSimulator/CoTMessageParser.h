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

#ifndef COTMESSAGEPARSER_H
#define COTMESSAGEPARSER_H

#include "AbstractMessageParser.h"

#include <QXmlStreamReader>
#include <QFile>

class CoTMessageParser : public AbstractMessageParser
{
  Q_OBJECT

public:
  explicit CoTMessageParser(const QString& filePath, QObject* parent = nullptr);
  ~CoTMessageParser();

  virtual QByteArray nextMessage() override;

  virtual void reset() override;

  virtual bool atEnd() const override;

private:
  Q_DISABLE_COPY(CoTMessageParser)
  CoTMessageParser() = delete;

  QFile m_device;
  QXmlStreamReader m_reader;
  bool m_isParsing = false;
};

#endif // COTMESSAGEPARSER_H
