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

#ifndef MARKUPBROADCAST_H
#define MARKUPBROADCAST_H

// toolkit headers
#include "AbstractTool.h"

class QJsonObject;
class QJsonDocument;

namespace Dsa
{

class DataSender;
class DataListener;

class MarkupBroadcast : public AbstractTool
{
  Q_OBJECT

public:
  explicit MarkupBroadcast(QObject* parent = nullptr);
  ~MarkupBroadcast();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  void broadcastMarkup(const QString& json);

signals:
  void markupReceived(const QString& filePath, const QString& sharedBy);
  void markupSent(const QString& filePath);

private:
  void updateDataSender();
  void updateDataListener();

  static const QString MARKUPCONFIG_PROPERTYNAME;
  static const QString ROOTDATA_PROPERTYNAME;
  static const QString UDPPORT_PROPERTYNAME;
  static const QString USERNAME_PROPERTYNAME;
  static const QString MARKUPKEY;
  static const QString NAMEKEY;
  static const QString SHAREDBYKEY;
  QString m_username;
  QString m_rootDataDirectory;
  DataSender* m_dataSender;
  DataListener* m_dataListener;
  int m_udpPort = -1;
};

} // Dsa

#endif // MARKUPBROADCAST_H
