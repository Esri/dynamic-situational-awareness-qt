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

// example app headers
#include "MarkupBroadcast.h"
#include "DataSender.h"
#include "DataListener.h"

// C++ API headers

// Qt headers
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUdpSocket>

using namespace Esri::ArcGISRuntime;

const QString MarkupBroadcast::MARKUPCONFIG_PROPERTYNAME = QStringLiteral("MarkupConfig");
const QString MarkupBroadcast::UDPPORT_PROPERTYNAME = QStringLiteral("port");
const QString MarkupBroadcast::USERNAME_PROPERTYNAME = QStringLiteral("UserName");
const QString MarkupBroadcast::NAMEKEY = QStringLiteral("name");
const QString MarkupBroadcast::MARKUPKEY = QStringLiteral("markup");
const QString MarkupBroadcast::SHAREDBYKEY = QStringLiteral("sharedBy");

/*
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

    // TODO ignore if from same username
    // TODO get operational data path from properties
    // write the JSON to disk
    const QJsonObject markupObject = markupJson.object();
    const QString sharedBy = markupObject.value(SHAREDBYKEY).toString();
    const QString markupName = markupObject.value(MARKUPKEY).toObject().value(NAMEKEY).toString();
    const QString markupFileName = QString("C:/Users/luca6804/ArcGIS/Runtime/Data/DSA/OperationalData/%1.markup").arg(markupName);
    QFile markupFile(markupFileName);
    if (markupFile.open(QIODevice::ReadWrite))
    {
      QTextStream stream(&markupFile);
      QString strJson(markupJson.toJson(QJsonDocument::Compact));
      stream << strJson << endl;
    }

    emit this->markupReceived(markupFileName, sharedBy);
  });
}

/*
 \brief Destructor
 */
MarkupBroadcast::~MarkupBroadcast()
{
}

/*
 \brief Returns the tool's name
 */
QString MarkupBroadcast::toolName() const
{
  return QStringLiteral("Markup Broadcast");
}

/*
 \brief Sets \a properties from the configuration file
 */
void MarkupBroadcast::setProperties(const QVariantMap& properties)
{
  m_username = properties[USERNAME_PROPERTYNAME].toString();

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
void MarkupBroadcast::broadcastMarkup(const QJsonObject& json)
{
  if (!m_dataSender)
    return;

  m_dataSender->sendData(QJsonDocument(json).toJson());
}

/*
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

/*
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
