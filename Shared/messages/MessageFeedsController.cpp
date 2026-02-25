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

// C++ API
#include "LayerListModel.h"
#include "Scene.h"
#include "SceneQuickView.h"
// DSA
#include "AppConstants.h"
#include "DataListener.h"
#include "LocationBroadcast.h"
#include "Message.h"
#include "MessageFeed.h"
#include "MessageFeedConstants.h"
#include "MessageFeedListModel.h"
#include "MessagesOverlay.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"
// Qt
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QUdpSocket>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString MessageFeedsController::RESOURCE_DIRECTORY_PROPERTYNAME = "ResourceDirectory";

/*!
  \class Dsa::MessageFeedsController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for working with the list of message feeds.
 */

/*!
  \brief Constructs a default MessageFeedsController with an optional \a parent.
 */
MessageFeedsController::MessageFeedsController(QObject* parent) :
  AbstractTool(parent),
  m_messageFeeds(new MessageFeedListModel(this)),
  m_locationBroadcast(new LocationBroadcast(this))
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged, this, &MessageFeedsController::setSceneFromGeoView);

  ToolManager::instance().addTool(this);
}

/*!
  \brief Destructor
 */
MessageFeedsController::~MessageFeedsController()
{
}

/*!
  \brief Sets the Scene from the ToolResourceProvider's GeoView to use during setupFeeds().
 */
void MessageFeedsController::setSceneFromGeoView()
{
  const auto* sceneView = static_cast<const SceneQuickView*>(ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  connect(sceneView, &SceneQuickView::sceneChanged, this, [this, sceneView]
  {
    auto* scene = sceneView->arcGISScene();
    if (!scene)
      return;

    m_scene = scene;
    if (m_messageFeeds->rowCount() == 0)
      setupFeeds();
  });
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

    messageFeed->addMessage(m);
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

void MessageFeedsController::setupFeeds()
{
  // parse and add message feeds
  const QJsonArray propertiesList = QJsonArray::fromVariantList(m_messageFeedProperties);
  std::for_each(std::cbegin(propertiesList), std::cend(propertiesList), [&](const QJsonValue& propertiesItem)
  {
    const QVariantMap properties = propertiesItem.toObject().toVariantMap();
    MessageFeed* feed = new MessageFeed(properties, m_resourcePath, this);
    if (!feed->configurationWasValid())
      return;

    connect(feed, &MessageFeed::doneLoading, this, [feed](const Esri::ArcGISRuntime::Error&)
    {
      feed->refreshOverlay();
    });
    m_scene->operationalLayers()->append(feed->messagesOverlay());
    m_messageFeeds->append(feed);
    connect(feed, &MessageFeed::feedChanged, this, &MessageFeedsController::notifyPropertyChanged);
  });

  if (m_messageFeeds->count() > 0)
  {
    m_selectedFeedIndex = 0;
    emit selectedFeedChanged(0);
  }

  // only needs to be cached until the geoView is ready
  m_messageFeedProperties.clear();
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
void MessageFeedsController::toolInitProperties(const QVariantMap& properties)
{
  using namespace MessageFeedConstants;
  setResourcePath(properties[RESOURCE_DIRECTORY_PROPERTYNAME].toString());
  m_messageFeedProperties = properties[MESSAGE_FEEDS_PROPERTYNAME].toList();

  auto userNameFindIt = properties.find(AppConstants::PROPERTYNAME_USERNAME);
  if (userNameFindIt != properties.end())
    m_locationBroadcast->setUserName(userNameFindIt.value().toString());

  // only add data listeners at startup
  if (m_dataListeners.isEmpty())
  {
    // parse and add data listeners on specified UDP ports
    const auto messageFeedUdpPorts = properties[MESSAGE_FEED_UDP_PORTS_PROPERTYNAME].toStringList();
    for (const auto& udpPort : messageFeedUdpPorts)
    {
      QUdpSocket* udpSocket = new QUdpSocket(this);
      udpSocket->bind(udpPort.toInt(), QUdpSocket::DontShareAddress | QUdpSocket::ReuseAddressHint);

      addDataListener(new DataListener(udpSocket, this));
    }
  }

  // only setup message feeds at startup
  if (m_scene && m_messageFeeds->rowCount() == 0)
    setupFeeds();

  const auto locationBroadcastConfig = properties[LOCATION_BROADCAST_CONFIG_PROPERTYNAME].toMap();
  if (locationBroadcastConfig.contains(LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE) &&
      locationBroadcastConfig.contains(LOCATION_BROADCAST_CONFIG_PORT))
  {
    m_locationBroadcast->setMessageType(locationBroadcastConfig.value(LOCATION_BROADCAST_CONFIG_MESSAGE_TYPE).toString());
    m_locationBroadcast->setUdpPort(locationBroadcastConfig.value(LOCATION_BROADCAST_CONFIG_PORT).toInt());
  }
}

bool MessageFeedsController::shouldSetProperties(const QString& propertyName)
{
  // list all property names that should cause the tool to re-initialize
  using namespace MessageFeedConstants;
  static const std::unordered_set<QString> propertyNames
  {
    RESOURCE_DIRECTORY_PROPERTYNAME,
    MESSAGE_FEEDS_PROPERTYNAME,
    AppConstants::PROPERTYNAME_USERNAME,
    MESSAGE_FEED_UDP_PORTS_PROPERTYNAME,
    LOCATION_BROADCAST_CONFIG_PROPERTYNAME,
    MESSAGE_FEEDS_SHOW_PREVIOUS_OBSERVATIONS,
    MESSAGE_FEEDS_SHOW_TRACK_LINE,
    MESSAGE_FEEDS_COLOR_OBSERVATIONS,
    MESSAGE_FEEDS_COLOR_TRACK_LINE,
  };

  return setContainsString(propertyNames, propertyName);
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

MessageFeed *MessageFeedsController::selectedFeed()
{
  return m_messageFeeds->at(m_selectedFeedIndex);
}

int MessageFeedsController::selectedFeedIndex() const
{
  return m_selectedFeedIndex;
}

void MessageFeedsController::setSelectedFeedIndex(int index)
{
  m_selectedFeedIndex = -1;

  const MessageFeed* feed = m_messageFeeds->at(index);
  if (feed)
    m_selectedFeedIndex = index;

  emit selectedFeedChanged(index);
}

void MessageFeedsController::notifyPropertyChanged()
{
  emit propertyChanged(MessageFeedConstants::MESSAGE_FEEDS_PROPERTYNAME, m_messageFeeds->toJsonArray());
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
