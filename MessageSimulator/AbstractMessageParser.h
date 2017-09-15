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

#ifndef ABSTRACTMESSAGEPARSER_H
#define ABSTRACTMESSAGEPARSER_H

#include <QObject>

class AbstractMessageParser : public QObject
{
  Q_OBJECT

public:
  ~AbstractMessageParser();

  virtual QByteArray nextMessage() = 0;

  virtual void reset() = 0;

  virtual bool atEnd() const = 0;

  QString filePath() const;

signals:
  void errorOccurred(const QString& error);

protected:
  explicit AbstractMessageParser(const QString& filePath, QObject* parent = nullptr);

private:
  Q_DISABLE_COPY(AbstractMessageParser)
  AbstractMessageParser() = delete;

  QString m_filePath;
};

#endif // ABSTRACTMESSAGEPARSER_H
