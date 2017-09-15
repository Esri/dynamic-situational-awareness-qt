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

#include "MessageSimulatorController.h"
#include "MessageSender.h"
#include "CoTMessageParser.h"
#include "SimulatedMessage.h"
#include "SimulatedMessageListModel.h"

#include <QSettings>

MessageSimulatorController::MessageSimulatorController(QObject* parent) :
  QObject(parent),
  m_messageSender(new MessageSender(this)),
  m_messages(new SimulatedMessageListModel(this))
{
  connect(&m_timer, &QTimer::timeout, this, [this]
  {
    if (m_messageParser->atEnd())
    {
      // reached end of the message parser
      // check if simulation is looped, if not end the simulation
      if (m_simulationLooped && m_messagesSent > 0)
      {
        // reset the message parser to the beginning to continue
        // looping through messages
        m_messageParser->reset();
      }
      else if (m_messagesSent == 0)
      {
        // if no messages have been sent and we've reached the end of the parser
        // then the simulation contains no messages
        emit errorOccurred("Simulation file contains no messages");
        stopSimulation();
        return;
      }
      else
      {
        // simulation has finished
        stopSimulation();
        return;
      }
    }

    const auto messageBytes = m_messageParser->nextMessage();
    if (messageBytes.isEmpty())
    {
      emit errorOccurred("Message is empty");
      return;
    }

    if (m_messageSender->sendMessage(messageBytes) == -1)
    {
      emit errorOccurred("Failed to send message");
      return;
    }

    m_messagesSent++;
  });

  connect(m_messageSender, &MessageSender::messageSent, this, [this](const QByteArray& message)
  {
    // create a simulated message to be added to the messages model
    SimulatedMessage* cotMessage = SimulatedMessage::createFromCoTMessage(message, this);
    if (!cotMessage)
    {
      emit errorOccurred("Failed to create simulated CoT message");
      return;
    }

    m_messages->append(cotMessage);
  });

  // load settings for the app if they exist
  loadSettings();
}

MessageSimulatorController::~MessageSimulatorController()
{
  // stop active simulation
  stopSimulation();
}

QUrl MessageSimulatorController::simulationFile() const
{
  return m_simulationFile;
}

void MessageSimulatorController::setPort(int port)
{
  if (m_port == port)
    return;

  m_port = port;

  emit portChanged();
}

int MessageSimulatorController::port() const
{
  return m_port;
}

void MessageSimulatorController::setMessageFrequency(int messageFrequency)
{
  const auto previousMessageFrequency = m_messageFrequency;

  if (messageFrequency > 0)
  {
    m_messageFrequency = messageFrequency;
    if (m_timer.isActive())
      m_timer.stop();

    if (m_simulationStarted && !m_simulationPaused)
    {
      float messageFrequencyInSeconds = (timeUnitToSeconds(m_timeUnit) / static_cast<float>(messageFrequency));
      m_timer.start(messageFrequencyInSeconds * 1000); // in ms
    }

    if (previousMessageFrequency != m_messageFrequency)
      emit messageFrequencyChanged();
  }
}

int MessageSimulatorController::messageFrequency() const
{
  return m_messageFrequency;
}

bool MessageSimulatorController::isSimulationStarted() const
{
  return m_simulationStarted;
}

bool MessageSimulatorController::isSimulationPaused() const
{
  return m_simulationPaused;
}

bool MessageSimulatorController::isSimulationLooped() const
{
  return m_simulationLooped;
}

void MessageSimulatorController::setSimulationLooped(bool simulationLooped)
{
  if (m_simulationLooped == simulationLooped)
    return;

  m_simulationLooped = simulationLooped;

  emit simulationLoopedChanged();
}

QString MessageSimulatorController::timeUnit() const
{
  return m_timeUnit;
}

void MessageSimulatorController::setTimeUnit(const QString& timeUnit)
{
  if (m_timeUnit == timeUnit)
    return;

  m_timeUnit = timeUnit;

  emit timeUnitChanged();
}

QAbstractListModel* MessageSimulatorController::messages() const
{
  return m_messages;
}

void MessageSimulatorController::startSimulation(const QUrl& file)
{
  // first stop the simulation if it was already running
  stopSimulation();

  // create UDP connection to broadcast address with specified port
  m_udpSocket = new QUdpSocket(this);
  m_udpSocket->connectToHost(QHostAddress::Broadcast, m_port, QIODevice::WriteOnly);
  m_messageSender->setDevice(m_udpSocket);

  if (m_messageParser)
    delete m_messageParser;

  // create a CoT message parser with specified input file
  m_messageParser = new CoTMessageParser(file.toLocalFile(), this);
  connect(m_messageParser, &AbstractMessageParser::errorOccurred, this, &MessageSimulatorController::errorOccurred);

  // clear the messages model
  m_messages->clear();

  if (m_simulationFile != file)
  {
    m_simulationFile = file;

    emit simulationFileChanged();
  }

  m_simulationStarted = true;
  m_simulationPaused = false;
  setMessageFrequency(m_messageFrequency);
  m_messagesSent = 0;

  emit simulationStartedChanged();

  // save app settings for next time the app is launched
  saveSettings();
}

void MessageSimulatorController::pauseSimulation()
{
  m_simulationPaused = true;
  m_timer.stop();

  emit simulationPausedChanged();
}

void MessageSimulatorController::resumeSimulation()
{
  m_simulationPaused = false;
  setMessageFrequency(m_messageFrequency);

  emit simulationPausedChanged();
}

void MessageSimulatorController::stopSimulation()
{
  // tear down the simulation
  if (!m_simulationStarted)
    return;

  m_simulationPaused = false;

  m_timer.stop();
  m_simulationStarted = false;

  if (m_udpSocket)
  {
    if (m_udpSocket->isOpen())
      m_udpSocket->close();

    delete m_udpSocket;
    m_udpSocket = nullptr;
  }

  emit simulationStartedChanged();
}

void MessageSimulatorController::sendMessage(const QString& message)
{
  m_messageSender->sendMessage(message.toUtf8());
}

void MessageSimulatorController::saveSettings()
{
  QSettings settings;
  settings.setValue("simulationFile", m_simulationFile);
  settings.setValue("port", m_port);
  settings.setValue("messageFrequency", m_messageFrequency);
  settings.setValue("timeUnit", m_timeUnit);
  settings.setValue("loop", m_simulationLooped);
}

void MessageSimulatorController::loadSettings()
{
  QSettings settings;
  m_simulationFile = settings.value("simulationFile", QUrl()).toUrl();
  m_port = settings.value("port", -1).toInt();
  m_messageFrequency = settings.value("messageFrequency", 1).toInt();
  m_timeUnit = settings.value("timeUnit", "second").toString();
  m_simulationLooped = settings.value("loop", true).toBool();
}

float MessageSimulatorController::timeUnitToSeconds(const QString& timeUnit)
{
  if (timeUnit.compare("minute") == 0)
  {
    return 60.0f;
  }
  else if (timeUnit.compare("hour") == 0)
  {
    return 60.0f * 60.0f;
  }
  else
  {
    //Default: seconds
    return 1.0f;
  }
}
