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
#include "MessageSender.h"

#include "ToolResourceProvider.h"
#include "ToolManager.h"

#include "DictionarySymbolStyle.h"
#include "DictionaryRenderer.h"
#include "SimpleRenderer.h"
#include "PictureMarkerSymbol.h"

#include <QUdpSocket>
#include <QHostInfo>
#include <QTimer>

using namespace Esri::ArcGISRuntime;

const QString MessageFeedsController::RESOURCE_DIRECTORY_PROPERTYNAME = "ResourceDirectory";
const QString MessageFeedsController::MESSAGE_FEED_UDP_PORTS_PROPERTYNAME = "MessageFeedUdpPorts";
const QString MessageFeedsController::MESSAGE_FEEDS_PROPERTYNAME = "MessageFeeds";
const QString MessageFeedsController::SEND_LOCATION_MESSAGE_FEED_PROPERTYNAME = "SendLocationMessageFeed";

MessageFeedsController::MessageFeedsController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_messageFeeds(new MessageFeedListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]
  {
    setGeoView(Toolkit::ToolResourceProvider::instance()->geoView());
  });
}

MessageFeedsController::~MessageFeedsController()
{
}

void MessageFeedsController::setGeoView(GeoView* geoView)
{
  m_geoView = geoView;
}

QAbstractListModel* MessageFeedsController::messageFeeds() const
{
  return m_messageFeeds;
}

QList<MessageListener*> MessageFeedsController::messageListeners() const
{
  return m_messageListeners;
}

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

    if (m.messageId() == m_sendLocationMessage.messageId()) // do not display current location message
      return;

    MessageFeed* messageFeed = m_messageFeeds->messageFeedByType(m.messageType());
    if (!messageFeed)
      return;

    messageFeed->messagesOverlay()->addMessage(m);
  });
}

void MessageFeedsController::removeMessageListener(MessageListener* messageListener)
{
  if (!messageListener)
    return;

  m_messageListeners.removeOne(messageListener);

  disconnect(messageListener, &MessageListener::messageReceived, this, nullptr);
}

QString MessageFeedsController::toolName() const
{
  return QStringLiteral("messages");
}

void MessageFeedsController::setProperties(const QVariantMap& properties)
{
  setDataPath(properties[RESOURCE_DIRECTORY_PROPERTYNAME].toString());

  // parse and add message listeners on specified UDP ports
  const auto messageFeedUdpPorts = properties[MESSAGE_FEED_UDP_PORTS_PROPERTYNAME].toStringList();
  for (const auto& udpPort : messageFeedUdpPorts)
  {
    QUdpSocket* udpSocket = new QUdpSocket(this);
    udpSocket->bind(udpPort.toInt(), QUdpSocket::DontShareAddress | QUdpSocket::ReuseAddressHint);

    addMessageListener(new MessageListener(udpSocket, this));
  }

  // parse and add message feeds
  const auto messageFeeds = properties[MESSAGE_FEEDS_PROPERTYNAME].toStringList();
  for (const auto& messageFeed : messageFeeds)
  {
    const auto& messageFeedConfig = messageFeed.split(":");
    if (messageFeedConfig.size() != 3)
      continue;

    const auto feedName = messageFeedConfig[0];
    const auto feedType = messageFeedConfig[1];
    const auto rendererInfo = messageFeedConfig[2];

    MessagesOverlay* overlay = new MessagesOverlay(m_geoView, createRenderer(rendererInfo, this));
    MessageFeed* feed = new MessageFeed(feedName, feedType, overlay, this);
    m_messageFeeds->append(feed);
  }

  const auto sendLocationMessageFeed = properties[SEND_LOCATION_MESSAGE_FEED_PROPERTYNAME].toStringList();
  if (sendLocationMessageFeed.size() == 2)
  {
    setSendLocationMessageFeedType(sendLocationMessageFeed.at(0));
    setSendLocationMessageFeedUdpPort(sendLocationMessageFeed.at(1).toInt());
  }
}

void MessageFeedsController::setDataPath(const QString& dataPath)
{
  if (dataPath == m_dataPath)
    return;

  m_dataPath = dataPath;

  emit propertyChanged(RESOURCE_DIRECTORY_PROPERTYNAME, dataPath);
}

QString MessageFeedsController::sendLocationMessageFeedType() const
{
  return m_sendLocationMessageFeedType;
}

void MessageFeedsController::setSendLocationMessageFeedType(const QString& messageFeedType)
{
  if (m_sendLocationMessageFeedType == messageFeedType)
    return;

  m_sendLocationMessageFeedType = messageFeedType;

  if (m_sendLocationMessageFeedType.isEmpty())
    setSendLocationMessageEnabled(false);
  else
    updateSendLocationMessage();
}

int MessageFeedsController::sendLocationMessageFeedUdpPort() const
{
  return m_sendLocationMessageFeedUdpPort;
}

void MessageFeedsController::setSendLocationMessageFeedUdpPort(int port)
{
  if (m_sendLocationMessageFeedUdpPort == port)
    return;

  m_sendLocationMessageFeedUdpPort = port;

  if (m_sendLocationMessageFeedUdpPort == -1)
    setSendLocationMessageEnabled(false);
  else
    updateSendLocationMessage();
}

bool MessageFeedsController::isSendLocationMessageEnabled() const
{
  return m_sendLocationMessageEnabled;
}

void MessageFeedsController::setSendLocationMessageEnabled(bool enabled)
{
  if (m_sendLocationMessageEnabled == enabled)
    return;

  m_sendLocationMessageEnabled = enabled;

  emit sendLocationMessageEnabledChanged();

  updateSendLocationMessage();
}

int MessageFeedsController::sendLocationMessageFrequency() const
{
  return m_sendLocationMessageFrequency;
}

void MessageFeedsController::setSendLocationMessageFrequency(int frequency)
{
  if (m_sendLocationMessageFrequency == frequency)
    return;

  m_sendLocationMessageFrequency = frequency;

  emit sendLocationMessageFrequencyChanged();

  if (m_sendLocationMessageTimer)
    m_sendLocationMessageTimer->setInterval(m_sendLocationMessageFrequency);
}

Renderer* MessageFeedsController::createRenderer(const QString& rendererInfo, QObject* parent) const
{
  // hold mil2525 symbol styles as statics to be shared by multiple renderers if needed
  static DictionarySymbolStyle* dictionarySymbolStyleMil2525c = nullptr;
  static DictionarySymbolStyle* dictionarySymbolStyleMil2525d = nullptr;

  if (rendererInfo.compare("mil2525c", Qt::CaseInsensitive) == 0)
  {
    if (!dictionarySymbolStyleMil2525c)
      dictionarySymbolStyleMil2525c = new DictionarySymbolStyle("mil2525c_b2", m_dataPath + "/styles/mil2525c_b2.stylx", parent);

    return new DictionaryRenderer(dictionarySymbolStyleMil2525c, parent);
  }
  else if (rendererInfo.compare("mil2525d", Qt::CaseInsensitive) == 0)
  {
    if (!dictionarySymbolStyleMil2525d)
      dictionarySymbolStyleMil2525d = new DictionarySymbolStyle("mil2525d", m_dataPath + "/styles/mil2525d.stylx", parent);

    return new DictionaryRenderer(dictionarySymbolStyleMil2525d, parent);
  }

  // else default to simple renderer with picture marker symbol
  PictureMarkerSymbol* symbol = new PictureMarkerSymbol(QImage(QString(":/Resources/icons/xhdpi/message/%1").arg(rendererInfo)), parent);
  symbol->setWidth(40.0f);
  symbol->setHeight(40.0f);
  return new SimpleRenderer(symbol, parent);
}

void MessageFeedsController::updateSendLocationMessage()
{
  if (m_locationChangedConn)
    disconnect(m_locationChangedConn);

  if (m_sendLocationMessageFeedType.isEmpty() || m_sendLocationMessageFeedUdpPort == -1)
    return;

  if (m_sendLocationMessageSender)
  {
    delete m_sendLocationMessageSender;
    m_sendLocationMessageSender = nullptr;
  }

  m_sendLocationMessageSender = new MessageSender(this);

  QUdpSocket* udpSocket = new QUdpSocket(m_sendLocationMessageSender);
  udpSocket->connectToHost(QHostAddress::Broadcast, m_sendLocationMessageFeedUdpPort, QIODevice::WriteOnly);
  m_sendLocationMessageSender->setDevice(udpSocket);

  m_sendLocationMessageTimer = new QTimer(m_sendLocationMessageSender);
  connect(m_sendLocationMessageTimer, &QTimer::timeout, this, [this]
  {
    if (!m_sendLocationMessageEnabled || !m_sendLocationMessageSender)
      return;

    if (m_currentLocation.isEmpty())
      return;

    if (m_sendLocationMessage.isEmpty())
    {
      QVariantMap attribs;

      m_sendLocationMessage = Message(Message::MessageAction::Update, m_currentLocation);
      m_sendLocationMessage.setMessageId(QUuid::createUuid().toString());
      m_sendLocationMessage.setMessageType(m_sendLocationMessageFeedType);
      m_sendLocationMessage.setSymbolId("SFGPEVAL-------");

      attribs.insert("sic", "SFGPEVAL-------");
      attribs.insert("uniquedesignation", QHostInfo::localHostName());
      attribs.insert("status911", 0);
      m_sendLocationMessage.setAttributes(attribs);
    }
    else
    {
      m_sendLocationMessage.setGeometry(m_currentLocation);
    }

    m_sendLocationMessageSender->sendMessage(m_sendLocationMessage.toGeoMessage());
  });

  m_sendLocationMessageTimer->start(m_sendLocationMessageFrequency);

  m_locationChangedConn = connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::locationChanged, this, [this](const Point& location)
  {
    if (!m_sendLocationMessageEnabled)
      return;

    m_currentLocation = location;
  });
}
