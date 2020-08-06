
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

// PCH header
#include "pch.hpp"

#include "MarkupBroadcast.h"

// dsa app headers
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
  \class Dsa::MarkupBroadcast
  \inmodule Dsa
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
      stream << strJson << Qt::endl;

      // process the markup differently if it is the one that you sent
      if (m_username == sharedBy)
        emit this->markupSent(markupFileName);
      else
        emit this->markupReceived(markupFileName, sharedBy);
    }
  });

  Toolkit::ToolManager::instance().addTool(this);
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
   \brief Broadcasts the markup JSON (\a json) over a UDP port.
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

// Signal Documentation
/*!
  \fn void MarkupBroadcast::markupReceived(const QString& filePath, const QString& sharedBy);
  \brief Signal emitted when a markup is received.

  The \a filePath to the JSON and the author that the markup was \a sharedBy are passed through
  as parameters.
 */

/*!
  \fn void MarkupBroadcast::markupSent(const QString& filePath);
  \brief Signal emitted when a markup is sent with the specified \a filePath.
 */
