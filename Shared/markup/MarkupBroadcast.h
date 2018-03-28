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

#ifndef MARKUPBROADCAST_H
#define MARKUPBROADCAST_H

class QJsonObject;
class QJsonDocument;
class DataSender;
class DataListener;

#include "AbstractTool.h"

class MarkupBroadcast : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:
  explicit MarkupBroadcast(QObject* parent = nullptr);
  ~MarkupBroadcast();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  void broadcastMarkup(const QJsonObject& json);

signals:
  void markupReceived(const QString& filePath, const QString& sharedBy);

private:
  void updateDataSender();
  void updateDataListener();

  static const QString MARKUPCONFIG_PROPERTYNAME;
  static const QString UDPPORT_PROPERTYNAME;
  static const QString USERNAME_PROPERTYNAME;
  static const QString MARKUPKEY;
  static const QString NAMEKEY;
  static const QString SHAREDBYKEY;
  QString m_username;
  DataSender* m_dataSender;
  DataListener* m_dataListener;
  int m_udpPort = -1;
};

#endif // MARKUPBROADCAST_H
