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

#ifndef SIMULATEDMESSAGE_H
#define SIMULATEDMESSAGE_H

// Qt headers
#include <QObject>

class SimulatedMessage : public QObject
{
  Q_OBJECT

public:
  static const QString COT_ROOT_ELEMENT_NAME;
  static const QString COT_ELEMENT_NAME;
  static const QString COT_TYPE_NAME;
  static const QString COT_UID_NAME;

  static const QString GEOMESSAGE_ROOT_ELEMENT_NAME;
  static const QString GEOMESSAGE_ELEMENT_NAME;
  static const QString GEOMESSAGE_ID_NAME;
  static const QString GEOMESSAGE_SIC_NAME;

  enum class MessageFormat
  {
    CoT = 0,
    GeoMessage = 1,
    Unknown = -1
  };

  explicit SimulatedMessage(QObject* parent = nullptr);
  SimulatedMessage(MessageFormat messageFormat, const QString& messageId, const QString& messageAction, const QString& symbolId, QObject* parent = nullptr);
  ~SimulatedMessage();

  static SimulatedMessage* create(const QByteArray& message, QObject* parent = nullptr);
  static SimulatedMessage* createFromCoTMessage(const QByteArray& message, QObject* parent = nullptr);
  static SimulatedMessage* createFromGeoMessage(const QByteArray& message, QObject* parent = nullptr);

  MessageFormat messageFormat() const;
  void setMessageFormat(MessageFormat messageFormat);

  QString messageFormatString() const;

  QString messageId() const;
  void setMessageId(const QString& messageId);

  QString messageAction() const;
  void setMessageAction(const QString& messageAction);

  QString symbolId() const;
  void setSymbolId(const QString& symbolId);

private:
  Q_DISABLE_COPY(SimulatedMessage)

  MessageFormat m_messageFormat = MessageFormat::Unknown;
  QString m_messageId;
  QString m_messageAction;
  QString m_symbolId;
};

#endif // SIMULATEDMESSAGE_H
