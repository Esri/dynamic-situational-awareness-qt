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

#ifndef ABSTRACTMESSAGEPARSER_H
#define ABSTRACTMESSAGEPARSER_H

// Qt headers
#include <QObject>

class AbstractMessageParser : public QObject
{
  Q_OBJECT

public:
  ~AbstractMessageParser();

  static AbstractMessageParser* createMessageParser(const QString& filePath, QObject* parent = nullptr);

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
