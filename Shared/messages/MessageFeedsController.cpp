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

#include "MessageFeedsController.h"

// example app headers
#include "AppConstants.h"
#include "DataListener.h"
#include "DataSender.h"
#include "LocationBroadcast.h"
#include "Message.h"
#include "MessageFeed.h"
#include "MessageFeedConstants.h"
#include "MessageFeedListModel.h"
#include "MessagesOverlay.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "DictionaryRenderer.h"
#include "DictionarySymbolStyle.h"
#include "PictureMarkerSymbol.h"
#include "SimpleRenderer.h"

// Qt headers
#include <QFileInfo>
#include <QJsonArray>
#include <QUdpSocket>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString MessageFeedsController::RESOURCE_DIRECTORY_PROPERTYNAME = "ResourceDirectory";

/*!
  \class Dsa::MessageFeedsController
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Tool controller for working with the list of message feeds.
 */

/*!
  \brief Constructs a default MessageFeedsController with an optional \a parent.
 */
MessageFeedsController::MessageFeedsController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_messageFeeds(new MessageFeedListModel(this)),
  m_locationBroadcast(new LocationBroadcast(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]
  {
    setGeoView(Toolkit::ToolResourceProvider::instance()->geoView());
  });
}

/*!
  \brief Destructor
 */
MessageFeedsController::~MessageFeedsController()
{
}

/*!
  \brief Sets the GeoView for the MessagesOverlay objects to \a geoView.
 */
void MessageFeedsController::setGeoView(GeoView* geoView)
{
  m_geoView = geoView;
}

/*!
  \property MessageFeedsController::messageFeeds
  \brief Returns the message feeds list model.
 */
QAbstractListModel* MessageFeedsController::messageFeeds() const
{
  return m_messageFeeds;
}

/*!
   \brief Returns the list of data listener objects that exist for
   the message feeds.
 */
QList<DataListener*> MessageFeedsController::dataListeners() const
{
  return m_dataListeners;
}

/*!
  \brief Adds and registers a data listener object to be used by the message feeds.

  \list
    \li \a dataListener - The data listener object to add to the controller.
  \endlist
 */
void MessageFeedsController::addDataListener(DataListener* dataListener)
{
  if (!dataListener)
    return;

  m_dataListeners.append(dataListener);

  connect(dataListener, &DataListener::dataReceived, this, [this](const QByteArray& data)
  {
    Message m = Message::create(data);
    if (m.isEmpty())
      return;

    if (m_locationBroadcast->isEnabled())
    {
      if (m_locationBroadcast->message().messageId() == m.messageId()) // do not display our own location broadcast message
        return;
    }

    MessageFeed* messageFeed = m_messageFeeds->messageFeedByType(m.messageType());
    if (!messageFeed)
      return;

    messageFeed->messagesOverlay()->addMessage(m);
  });
}

/*!
  \brief Removes a data listener object from the controller.

  \list
    \li \a dataListener - The data listener object to remove from the controller.
  \endlist
 */
void MessageFeedsController::removeDataListener(DataListener* dataListener)
{
  if (!dataListener)
    return;

  m_dataListeners.removeOne(dataListener);

  disconnect(dataListener, &DataListener::dataReceived, this, nullptr);
}

/*!
  \brief Returns the name of the message feeds controller.
 */
QString MessageFeedsController::toolName() const
{
  return QStringLiteral("messages");
}

/*!
  \brief Sets \a properties for configuring the message feeds controller.

  Applicable properties are:
  \list
    \li \c ResourceDirectory - The resource directory where symbol style files are located.
    \li \c MessageFeedUdpPorts - The UDP ports for listening to message feeds.
    \li \c MessageFeeds - A list of message feed configurations.
    \li \c LocationBroadcastConfig - The location broadcast configuration details.
    \li \c UserName - the name of the user to be broadcast.
  \endlist
 */
void MessageFeedsController::setProperties(const QVariantMap& properties)
{
  setResourcePath(properties[RESOURCE_DIRECTORY_PROPERTYNAME].toString());

  auto userNameFindIt = properties.find(AppConstants::USERNAME_PROPERTYNAME);
  if (userNameFindIt != properties.end())
    m_locationBroadcast->setUserName(userNameFindIt.value().toString());

  // only add data listeners at startup
  if (m_dataListeners.isEmpty())
  {
    // parse and add data listeners on specified UDP ports
    const auto messageFeedUdpPorts = properties[MessageFeedConstants::MESSAGE_FEED_UDP_PORTS_PROPERTYNAME].toStringList();
    for (const auto& udpPort : messageFeedUdpPorts)
    {
      QUdpSocket* udpSocket = new QUdpSocket(this);
      udpSocket->bind(udpPort.toInt(), QUdpSocket::DontShareAddress | QUdpSocket::ReuseAddressHint);

      addDataListener(new DataListener(udpSocket, this));
    }
  }

  // only setup message feeds at startup
  if (m_messageFeeds->rowCount() == 0)
  {
    // parse and add message feeds
    const auto messageFeeds = properties[MessageFeedConstants::MESSAGE_FEEDS_PROPERTYNAME].toList();
    const auto messageFeedsJson = QJsonArray::fromVariantList(messageFeeds);
    for (const auto& messageFeed : messageFeedsJson)
    {
      const auto messageFeedJsonObject = messageFeed.toObject();
      if (messageFeedJsonObject.size() < 4)
      {
        emit toolErrorOccurred(QStringLiteral("Invalid Message JSON recieved"), messageFeed.toString());
        continue;
      }

      const auto feedName = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_NAME].toString();
      const auto feedType = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_TYPE].toString();
      const auto rendererInfo = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_RENDERER].toString();
      const auto rendererThumbnail = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_THUMBNAIL].toString();
      const auto surfacePlacement = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT].toString();

      MessagesOverlay* overlay = new MessagesOverlay(m_geoView, createRenderer(rendererInfo, this), feedType, toSurfacePlacement(surfacePlacement), this);
      MessageFeed* feed = new MessageFeed(feedName, feedType, overlay, this);

      if (!rendererThumbnail.isEmpty())
      {
        if (QFile::exists(QString(":/Resources/icons/xhdpi/message/%1").arg(rendererThumbnail)))
          feed->setThumbnailUrl(QString("qrc:/Resources/icons/xhdpi/message/%1").arg(rendererThumbnail));
        else if (QFile::exists(m_resourcePath + QString("/icons/%1").arg(rendererThumbnail)))
          feed->setThumbnailUrl(QUrl::fromLocalFile(m_resourcePath + QString("/icons/%1").arg(rendererThumbnail)));
        else
          emit toolErrorOccurred(QString("Failed to find icon %1").arg(rendererThumbnail), QString("Could not find icon %1 for feed %2").arg(rendererThumbnail, feedName));
      }

      m_messageFeeds->append(feed);
    }
  }

  const auto locationBroadcastConfig = properties[MessageFeedConstants::LOCATION_BROADCAST_CONFIG_PROPERTYNAME].toMap();
  if (locationBroadcastConfig.contains(MessageFeedConstants::LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE) &&
      locationBroadcastConfig.contains(MessageFeedConstants::LOCATION_BROADCAST_CONFIG_PORT))
  {
    m_locationBroadcast->setMessageType(locationBroadcastConfig.value(MessageFeedConstants::LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE).toString());
    m_locationBroadcast->setUdpPort(locationBroadcastConfig.value(MessageFeedConstants::LOCATION_BROADCAST_CONFIG_PORT).toInt());
  }
}

/*!
  \brief Sets the data path to be used for symbol style resources as \a resourcePath.
 */
void MessageFeedsController::setResourcePath(const QString& resourcePath)
{
  if (!QFileInfo::exists(resourcePath))
  {
    emit toolErrorOccurred(QStringLiteral("Resource path not found"), QString("Failed to find %1").arg(resourcePath));
    return;
  }

  if (resourcePath == m_resourcePath)
    return;

  m_resourcePath = resourcePath;

  emit propertyChanged(RESOURCE_DIRECTORY_PROPERTYNAME, resourcePath);
}

LocationBroadcast* MessageFeedsController::locationBroadcast() const
{
  return m_locationBroadcast;
}

/*!
  \property MessageFeedsController::locationBroadcastEnabled
  \brief Returns \c true if the location broadcast is enabled.

  \sa LocationBroadcast::isEnabled
 */
bool MessageFeedsController::isLocationBroadcastEnabled() const
{
  return m_locationBroadcast->isEnabled();
}

/*!
  \brief Sets whether the location broadcast is \a enabled.

  \sa LocationBroadcast::setEnabled
 */
void MessageFeedsController::setLocationBroadcastEnabled(bool enabled)
{
  if (m_locationBroadcast->isEnabled() == enabled)
    return;

  m_locationBroadcast->setEnabled(enabled);

  emit locationBroadcastEnabledChanged();
}

/*!
  \property MessageFeedsController::locationBroadcastFrequency
  \brief Returns the location broadcast frequency.

  \sa LocationBroadcast::frequency
 */
int MessageFeedsController::locationBroadcastFrequency() const
{
  return m_locationBroadcast->frequency();
}

/*!
  \brief Sets the location broadcast message frequency to \a frequency.

  \sa LocationBroadcast::setFrequency
 */
void MessageFeedsController::setLocationBroadcastFrequency(int frequency)
{
  if (m_locationBroadcast->frequency() == frequency)
    return;

  m_locationBroadcast->setFrequency(frequency);

  emit locationBroadcastFrequencyChanged();
}

/*!
  \property MessageFeedsController::locationBroadcastInDistress
  \brief Returns \c true if the location broadcast reports
  message status as being in distress.
 */
bool MessageFeedsController::isLocationBroadcastInDistress() const
{
  return m_locationBroadcast->isInDistress();
}

/*!
  \brief Sets the location broadcast to report
  message status as being in distress to \a inDistress.

  \sa LocationBroadcast::setInDistress
 */
void MessageFeedsController::setLocationBroadcastInDistress(bool inDistress)
{
  if (m_locationBroadcast->isInDistress() == inDistress)
    return;

  m_locationBroadcast->setInDistress(inDistress);

  emit locationBroadcastInDistressChanged();
}

SurfacePlacement MessageFeedsController::toSurfacePlacement(const QString& surfacePlacement)
{
  if (surfacePlacement.compare("relative", Qt::CaseInsensitive) == 0)
    return SurfacePlacement::Relative;

  if (surfacePlacement.compare("absolute", Qt::CaseInsensitive) == 0)
    return SurfacePlacement::Absolute;

  return SurfacePlacement::Draped; // default
}

/*!
  \internal
  \brief Creates and returns a renderer from the provided \a rendererInfo with an optional \a parent.

  The \a rendererInfo parameter can be the symbol specification type (i.e. "mil2525c_b2" or "mil2525d") or
  it can be the name of an image file located in:

  \list
    \li the ":/Resources/icons/xhdpi/message" path, such
    as ":/Resources/icons/xhdpi/message/observation1600.png".
    \li an "icons" sub-directory under the \l resourcePath directory
  \endlist
 */
Renderer* MessageFeedsController::createRenderer(const QString& rendererInfo, QObject* parent) const
{
  // hold mil2525 symbol styles as statics to be shared by multiple renderers if needed
  static DictionarySymbolStyle* dictionarySymbolStyleMil2525c = nullptr;
  static DictionarySymbolStyle* dictionarySymbolStyleMil2525d = nullptr;

  if (rendererInfo.compare("mil2525c", Qt::CaseInsensitive) == 0)
  {
    if (!dictionarySymbolStyleMil2525c)
    {
      const auto stylePath = m_resourcePath + "/styles/mil2525c_b2.stylx";
      if (!QFileInfo::exists(stylePath))
      {
        emit const_cast<MessageFeedsController*>(this)->toolErrorOccurred(QStringLiteral("mil2525c_b2.stylx not found"), QString("Could not find %1").arg(stylePath));
        return nullptr;
      }

      dictionarySymbolStyleMil2525c = new DictionarySymbolStyle("mil2525c_b2", stylePath, parent);
    }

    return new DictionaryRenderer(dictionarySymbolStyleMil2525c, parent);
  }
  else if (rendererInfo.compare("mil2525d", Qt::CaseInsensitive) == 0)
  {
    if (!dictionarySymbolStyleMil2525d)
    {
      const auto stylePath = m_resourcePath + "/styles/mil2525d.stylx";
      if (!QFileInfo::exists(stylePath))
      {
        emit const_cast<MessageFeedsController*>(this)->toolErrorOccurred(QStringLiteral("mil2525d.stylx not found"), QString("Could not find %1").arg(stylePath));
        return nullptr;
      }

      dictionarySymbolStyleMil2525d = new DictionarySymbolStyle("mil2525d", m_resourcePath + "/styles/mil2525d.stylx", parent);
    }

    return new DictionaryRenderer(dictionarySymbolStyleMil2525d, parent);
  }

  // else default to simple renderer with picture marker symbol
  PictureMarkerSymbol* symbol = nullptr;
  const QString qrcFile = QString(":/Resources/icons/xhdpi/message/%1").arg(rendererInfo);

  if (QFile::exists(qrcFile))
  {
    symbol = new PictureMarkerSymbol(QImage(qrcFile), parent);
  }
  else
  {
    const QString dataFile = m_resourcePath + QString("/icons/%1").arg(rendererInfo);
    if (QFile::exists(dataFile))
      symbol = new PictureMarkerSymbol(QImage(dataFile), parent);
  }

  if (symbol == nullptr)
    return nullptr;

  symbol->setWidth(40.0f);
  symbol->setHeight(40.0f);
  return new SimpleRenderer(symbol, parent);
}

// Properties:

// Signals:
/*!
  \fn void MessageFeedsController::locationBroadcastEnabledChanged();
  \brief Signal emitted when the \l locationBroadcastEnabled property changes.
 */

/*!
  \fn void MessageFeedsController::locationBroadcastFrequencyChanged();
  \brief Signal emitted when the \l locationBroadcastFrequency property changes.
 */

/*!
  \fn void MessageFeedsController::locationBroadcastInDistressChanged();
  \brief Signal emitted when the \l locationBroadcastInDistress property changes.
 */

} // Dsa

/*!
  \fn void MessageFeedsController::toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  \brief Signal emitted when an error occurs.

  An \a errorMessage and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */
