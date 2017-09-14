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
