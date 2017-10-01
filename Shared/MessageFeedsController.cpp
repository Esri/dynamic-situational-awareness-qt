// Copyright 2016 ESRI
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
#include "DsaUtility.h"
#include "Message.h"
#include "MessagesOverlay.h"
#include "MessageListener.h"
#include "MessageFeedListModel.h"
#include "MessageFeed.h"

#include "ToolManager.h"

#include "DictionarySymbolStyle.h"

#include <QUdpSocket>

using namespace Esri::ArcGISRuntime;

MessageFeedsController::MessageFeedsController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_messageFeeds(new MessageFeedListModel(this))
{
  Toolkit::ToolManager::instance()->addTool(this);
}

MessageFeedsController::~MessageFeedsController()
{
}

// Instead of an init() function, it would be better to place a reference to the
// GeoView into the ToolManager.  If the GeoView changes a signal could be
// emitted which reconfigures the message feeds
void MessageFeedsController::init(GeoView* geoView)
{
  m_geoView = geoView;

  QString dataPath = DsaUtility::dataPath();

  //TODO: the following configuration should be read from the app configuration file
  //and the message feeds are built up on the fly rather than hardcoded

  constexpr int broadcastPort{45678};

  // set up the messages overlay with a Mil2525c_b2 dictionary style
  DictionarySymbolStyle* dictionarySymbolStyle = new DictionarySymbolStyle("mil2525c_b2", dataPath + "/styles/mil2525c_b2.stylx", this);
  MessagesOverlay* messagesOverlay = new MessagesOverlay(geoView, dictionarySymbolStyle, this);

  // create the messaging socket connection and hook up message receiving
  QUdpSocket* udpSocket = new QUdpSocket(this);
  udpSocket->bind(broadcastPort, QUdpSocket::DontShareAddress | QUdpSocket::ReuseAddressHint);

  MessageListener* messageListener = new MessageListener(udpSocket, this);
  connect(messageListener, &MessageListener::messageReceived, this, [this, messagesOverlay](const QByteArray& message)
  {
    Message cotMessage = Message::createFromCoTMessage(message);
    if (message.isEmpty())
      return;

    messagesOverlay->addMessage(cotMessage);
  });

  MessageFeed* messageFeed = new MessageFeed("CoT tracks", Message::MessageType::Track, "CoT", messagesOverlay, messageListener, this);
  m_messageFeeds->append(messageFeed);
}

QAbstractListModel* MessageFeedsController::messageFeeds() const
{
  return m_messageFeeds;
}

QString MessageFeedsController::toolName() const
{
  return QStringLiteral("messages");
}
