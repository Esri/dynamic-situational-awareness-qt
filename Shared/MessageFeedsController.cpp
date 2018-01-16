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

#include "ToolResourceProvider.h"
#include "ToolManager.h"

#include "DictionarySymbolStyle.h"
#include "DictionaryRenderer.h"
#include "SimpleRenderer.h"
#include "PictureMarkerSymbol.h"

#include <QUdpSocket>

using namespace Esri::ArcGISRuntime;

const QString MessageFeedsController::RESOURCE_DIRECTORY_PROPERTYNAME = "ResourceDirectory";
const QString MessageFeedsController::MESSAGE_FEED_UDP_PORTS_PROPERTYNAME = "MessageFeedUdpPorts";
const QString MessageFeedsController::MESSAGE_FEEDS_PROPERTYNAME = "MessageFeeds";

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
}

void MessageFeedsController::setDataPath(const QString& dataPath)
{
  if (dataPath == m_dataPath)
    return;

  m_dataPath = dataPath;

  emit propertyChanged(RESOURCE_DIRECTORY_PROPERTYNAME, dataPath);
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
