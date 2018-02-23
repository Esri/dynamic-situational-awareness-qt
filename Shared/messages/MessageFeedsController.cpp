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

#include "MessageFeedsController.h"
#include "Message.h"
#include "MessagesOverlay.h"
#include "MessageListener.h"
#include "MessageFeedListModel.h"
#include "MessageFeed.h"
#include "MessageFeedConstants.h"
#include "MessageSender.h"
#include "LocationBroadcast.h"

#include "ToolResourceProvider.h"
#include "ToolManager.h"

#include "DictionarySymbolStyle.h"
#include "DictionaryRenderer.h"
#include "SimpleRenderer.h"
#include "PictureMarkerSymbol.h"

#include <QFileInfo>
#include <QUdpSocket>
#include <QJsonArray>

using namespace Esri::ArcGISRuntime;

const QString MessageFeedsController::RESOURCE_DIRECTORY_PROPERTYNAME = "ResourceDirectory";

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
   \brief Sets the GeoView for the MessageOverlay objects to \a geoView.
 */
void MessageFeedsController::setGeoView(GeoView* geoView)
{
  m_geoView = geoView;
}

/*!
   \brief Returns the message feeds list model.
 */
QAbstractListModel* MessageFeedsController::messageFeeds() const
{
  return m_messageFeeds;
}

/*!
   \brief Returns the list of message listener objects that exist for
   the message feeds.
 */
QList<MessageListener*> MessageFeedsController::messageListeners() const
{
  return m_messageListeners;
}

/*!
   \brief Adds and registers a message listener object to be used by the message feeds.

   \list
     \li \a messageListener - The message listener object to add to the controller.
   \endlist
 */
void MessageFeedsController::addMessageListener(MessageListener* messageListener)
{
  if (!messageListener)
    return;

  m_messageListeners.append(messageListener);

  connect(messageListener, &MessageListener::messageReceived, this, [this](const QByteArray& message)
  {
    Message m = Message::create(message);
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
   \brief Removes a message listener object from the controller.

   \list
     \li \a messageListener - The message listener object to remove from the controller.
   \endlist
 */
void MessageFeedsController::removeMessageListener(MessageListener* messageListener)
{
  if (!messageListener)
    return;

  m_messageListeners.removeOne(messageListener);

  disconnect(messageListener, &MessageListener::messageReceived, this, nullptr);
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
     \li RESOURCE_DIRECTORY_PROPERTYNAME - The resource directory where symbol style files are located.
     \li MESSAGE_FEED_UDP_PORTS_PROPERTYNAME - The UDP ports for listening to message feeds.
     \li MESSAGE_FEEDS_PROPERTYNAME - A list of message feed configurations.
     \li LOCATION_BROADCAST_CONFIG_PROPERTYNAME - The location broadcast configuration details.
   \endlist
 */
void MessageFeedsController::setProperties(const QVariantMap& properties)
{
  setResourcePath(properties[RESOURCE_DIRECTORY_PROPERTYNAME].toString());

  // parse and add message listeners on specified UDP ports
  const auto messageFeedUdpPorts = properties[MessageFeedConstants::MESSAGE_FEED_UDP_PORTS_PROPERTYNAME].toStringList();
  for (const auto& udpPort : messageFeedUdpPorts)
  {
    QUdpSocket* udpSocket = new QUdpSocket(this);
    udpSocket->bind(udpPort.toInt(), QUdpSocket::DontShareAddress | QUdpSocket::ReuseAddressHint);

    addMessageListener(new MessageListener(udpSocket, this));
  }

  // parse and add message feeds
  const auto messageFeeds = properties[MessageFeedConstants::MESSAGE_FEEDS_PROPERTYNAME].toList();
  const auto messageFeedsJson = QJsonArray::fromVariantList(messageFeeds);
  for (const auto& messageFeed : messageFeedsJson)
  {
    const auto messageFeedJsonObject = messageFeed.toObject();
    if (messageFeedJsonObject.size() != 4)
    {
      emit toolErrorOccurred(QStringLiteral("Invalid Message JSON recieved"), messageFeed.toString());
      continue;
    }

    const auto feedName = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_NAME].toString();
    const auto feedType = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_TYPE].toString();
    const auto rendererObject = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_RENDERER].toObject();
    const auto rendererStyle = rendererObject[MessageFeedConstants::MESSAGE_FEEDS_RENDERER_STYLE].toString();
    const auto rendererIcon = rendererObject[MessageFeedConstants::MESSAGE_FEEDS_RENDERER_ICON].toString();
    const auto surfacePlacement = messageFeedJsonObject[MessageFeedConstants::MESSAGE_FEEDS_PLACEMENT].toString();

    MessagesOverlay* overlay = new MessagesOverlay(m_geoView, createRenderer(rendererStyle.isEmpty() ? rendererIcon : rendererStyle, this), toSurfacePlacement(surfacePlacement), this);
    MessageFeed* feed = new MessageFeed(feedName, feedType, overlay, this);

    if (!rendererIcon.isEmpty())
    {
      if (QFile::exists(QString(":/Resources/icons/xhdpi/message/%1").arg(rendererIcon)))
        feed->setIconUrl(QString("qrc:/Resources/icons/xhdpi/message/%1").arg(rendererIcon));
      else if (QFile::exists(m_resourcePath + QString("/icons/%1").arg(rendererIcon)))
        feed->setIconUrl(QUrl::fromLocalFile(m_resourcePath + QString("/icons/%1").arg(rendererIcon)));
    }

    m_messageFeeds->append(feed);
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
   as ":/Resources/icons/xhdpi/message/enemycontact1600.png".
    \li an "icons" sub-directory under the \l resourcePath directory
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
        emit toolErrorOccurred(QStringLiteral("mil2525c_b2.stylx not found"), QString("Could not find %1").arg(stylePath));
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
        emit toolErrorOccurred(QStringLiteral("mil2525d.stylx not found"), QString("Could not find %1").arg(stylePath));
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
/*!
  \property MessageFeedsController::messageFeeds
  \brief The message feeds list model (read-only).
*/

/*!
  \property MessageFeedsController::locationBroadcastEnabled
  \brief Whether the location broadcast is enabled or not.
*/

/*!
  \property MessageFeedsController::locationBroadcastFrequency
  \brief The location broadcast frequency.
*/

/*!
  \property MessageFeedsController::locationBroadcastInDistress
  \brief Whether the location broadcast reports message status as being in distress.
*/

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
