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

// example app headers
#include "CoTMessageParser.h"
#include "DataSender.h"
#include "SimulatedMessage.h"
#include "SimulatedMessageListModel.h"

// Qt headers
#include <QSettings>

MessageSimulatorController::MessageSimulatorController(QObject* parent) :
  QObject(parent),
  m_dataSender(new Dsa::Utilities::DataSender(this)),
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
        emit errorOccurred(tr("Simulation file contains no messages"));
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
      emit errorOccurred(tr("Message is empty"));
      return;
    }

    if (m_dataSender->sendData(messageBytes) == -1)
    {
      emit errorOccurred(tr("Failed to send message"));
      return;
    }

    m_messagesSent++;
  });

  connect(m_dataSender, &Dsa::Utilities::DataSender::dataSent, this, [this](const QByteArray& data)
  {
    // create a simulated message to be added to the messages model
    SimulatedMessage* cotMessage = SimulatedMessage::createFromCoTMessage(data, this);
    if (!cotMessage)
    {
      emit errorOccurred(tr("Failed to create simulated CoT message"));
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

void MessageSimulatorController::setSimulationFile(const QUrl& simulationFile)
{
  if (m_simulationFile == simulationFile)
    return;

  m_simulationFile = simulationFile;

  emit simulationFileChanged();
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

void MessageSimulatorController::setMessageFrequency(float messageFrequency)
{
  const auto previousMessageFrequency = m_messageFrequency;

  if (messageFrequency > 0)
  {
    m_messageFrequency = messageFrequency;
    if (m_timer.isActive())
      m_timer.stop();

    if (m_simulationStarted && !m_simulationPaused)
    {
      float messageFrequencyInSeconds = (timeUnitToSeconds(m_timeUnit) / messageFrequency);
      m_timer.start(messageFrequencyInSeconds * 1000.0f); // in ms
    }

    if (previousMessageFrequency != m_messageFrequency)
      emit messageFrequencyChanged();
  }
}

float MessageSimulatorController::messageFrequency() const
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

MessageSimulatorController::TimeUnit MessageSimulatorController::timeUnit() const
{
  return m_timeUnit;
}

void MessageSimulatorController::setTimeUnit(TimeUnit timeUnit)
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
  m_dataSender->setDevice(m_udpSocket);

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
  m_dataSender->sendData(message.toUtf8());
}

void MessageSimulatorController::saveSettings()
{
  QSettings settings;
  settings.setValue("simulationFile", m_simulationFile);
  settings.setValue("port", m_port);
  settings.setValue("messageFrequency", m_messageFrequency);
  settings.setValue("timeUnit", fromTimeUnit(m_timeUnit));
  settings.setValue("loop", m_simulationLooped);
}

void MessageSimulatorController::loadSettings()
{
  QSettings settings;
  setSimulationFile(settings.value("simulationFile", QUrl()).toUrl());
  setPort(settings.value("port", -1).toInt());
  setMessageFrequency(settings.value("messageFrequency", 1.0f).toFloat());
  setTimeUnit(toTimeUnit(settings.value("timeUnit", "seconds").toString()));
  setSimulationLooped(settings.value("loop", true).toBool());
}

QString MessageSimulatorController::fromTimeUnit(TimeUnit timeUnit)
{
  switch (timeUnit)
  {
  case TimeUnit::Seconds:
    return "second";
  case TimeUnit::Minutes:
    return "minute";
  case TimeUnit::Hours:
    return "hour";
  default: break;
  }

  return QString();
}

MessageSimulatorController::TimeUnit MessageSimulatorController::toTimeUnit(const QString& timeUnit)
{
  if (timeUnit.compare("second", Qt::CaseInsensitive) == 0)
    return TimeUnit::Seconds;
  if (timeUnit.compare("minute", Qt::CaseInsensitive) == 0)
    return TimeUnit::Minutes;
  if (timeUnit.compare("hour", Qt::CaseInsensitive) == 0)
    return TimeUnit::Hours;

  return TimeUnit::Seconds; // default to seconds
}

float MessageSimulatorController::timeUnitToSeconds(TimeUnit timeUnit)
{
  switch (timeUnit)
  {
  case TimeUnit::Seconds:
    return 1.0f;
  case TimeUnit::Minutes:
    return 60.0f;
  case TimeUnit::Hours:
    return 60.0f * 60.0f;
  default: break;
  }

  return 1.0f; // default to seconds
}
