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

// toolkit headers
#include "AbstractTool.h"

class QJsonObject;
class QJsonDocument;

namespace Dsa
{

class DataSender;
class DataListener;

class MarkupBroadcast : public Esri::ArcGISRuntime::Toolkit::AbstractTool
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
