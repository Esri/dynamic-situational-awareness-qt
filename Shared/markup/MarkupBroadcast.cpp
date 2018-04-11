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

// PCH header
#include "pch.hpp"

#include "MarkupBroadcast.h"

// example app headers
#include "DataListener.h"
#include "DataSender.h"

// Qt headers
#include <QDateTime>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUdpSocket>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString MarkupBroadcast::MARKUPCONFIG_PROPERTYNAME = QStringLiteral("MarkupConfig");
const QString MarkupBroadcast::ROOTDATA_PROPERTYNAME = QStringLiteral("RootDataDirectory");
const QString MarkupBroadcast::UDPPORT_PROPERTYNAME = QStringLiteral("port");
const QString MarkupBroadcast::USERNAME_PROPERTYNAME = QStringLiteral("UserName");
const QString MarkupBroadcast::NAMEKEY = QStringLiteral("name");
const QString MarkupBroadcast::MARKUPKEY = QStringLiteral("markup");
const QString MarkupBroadcast::SHAREDBYKEY = QStringLiteral("sharedBy");

/*!
  \class MarkupBroadcast
  \inherits Toolkit::AbstractTool
  \brief Tool controller for broadcasting markups.

  \sa DataSender
  \sa DataListener
 */

/*!
 \brief Constructor that takes an optional \a parent.
 */
MarkupBroadcast::MarkupBroadcast(QObject *parent) :
  Toolkit::AbstractTool(parent),
  m_dataSender(new DataSender(parent)),
  m_dataListener(new DataListener(parent))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(m_dataListener, &DataListener::dataReceived, this, [this](const QByteArray& data)
  {
    QJsonDocument markupJson = QJsonDocument::fromJson(data);

    // write the JSON to disk
    const QJsonObject markupObject = markupJson.object();
    const QString sharedBy = markupObject.value(SHAREDBYKEY).toString();

    const QString markupName = markupObject.value(MARKUPKEY).toObject().value(NAMEKEY).toString();
    const QString markupFolderName = QString("%1/OperationalData").arg(m_rootDataDirectory);
    QString markupFileName = QString("%1/%2.markup").arg(markupFolderName, markupName);
    QFileInfo fileInfo(markupFileName);
    if (fileInfo.exists())
      markupFileName = QString("%1/%2_%3.markup").arg(markupFolderName, markupName, QString::number(QDateTime::currentDateTime().currentMSecsSinceEpoch()));

    QFile markupFile(markupFileName);
    if (markupFile.open(QIODevice::ReadWrite))
    {
      QTextStream stream(&markupFile);
      QString strJson(markupJson.toJson(QJsonDocument::Compact));
      stream << strJson << endl;

      // process the markup differently if it is the one that you sent
      if (m_username == sharedBy)
        emit this->markupSent(markupFileName);
      else
        emit this->markupReceived(markupFileName, sharedBy);
    }
  });
}

/*!
 \brief Destructor
 */
MarkupBroadcast::~MarkupBroadcast()
{
}

/*!
 \brief Returns the tool's name
 */
QString MarkupBroadcast::toolName() const
{
  return QStringLiteral("Markup Broadcast");
}

/*!
 \brief Sets \a properties from the configuration file
 */
void MarkupBroadcast::setProperties(const QVariantMap& properties)
{
  m_username = properties[USERNAME_PROPERTYNAME].toString();

  m_rootDataDirectory = properties[ROOTDATA_PROPERTYNAME].toString();

  const auto markupPortConfig = properties[MARKUPCONFIG_PROPERTYNAME].toMap();
  auto findPortIt = markupPortConfig.find(UDPPORT_PROPERTYNAME);
  if (findPortIt != markupPortConfig.end())
  {
    bool ok = false;
    int newPort = findPortIt.value().toInt(&ok);
    if (ok)
      m_udpPort = newPort;
  }
  updateDataSender();
  updateDataListener();
}

/*!
   \brief Broadcasts the markup JSON over a UDP port.
 */
void MarkupBroadcast::broadcastMarkup(const QString& json)
{
  if (!m_dataSender)
    return;

  m_dataSender->sendData(json.toUtf8());
}

/*!
 \brief Updates the UDP Socket used for the DataSender.
 */
void MarkupBroadcast::updateDataSender()
{
  if (!m_dataSender)
    return;

  QUdpSocket* udpSocket = new QUdpSocket(m_dataSender);
  udpSocket->connectToHost(QHostAddress::Broadcast, m_udpPort, QIODevice::WriteOnly);
  m_dataSender->setDevice(udpSocket);
}

/*!
 \brief Updates the UDP Socket used for the DataListener.
 */
void MarkupBroadcast::updateDataListener()
{
  if (!m_dataListener)
    return;

  QUdpSocket* udpSocket = new QUdpSocket(this);
  udpSocket->bind(m_udpPort, QUdpSocket::DontShareAddress | QUdpSocket::ReuseAddressHint);
  m_dataListener->setDevice(udpSocket);
}

} // Dsa
