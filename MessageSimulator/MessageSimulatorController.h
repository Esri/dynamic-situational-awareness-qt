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

#ifndef MESSAGESIMULATORCONTROLLER_H
#define MESSAGESIMULATORCONTROLLER_H


// Qt headers
#include <QAbstractListModel>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QUrl>

namespace Dsa {
class DataSender;
}

class AbstractMessageParser;
class SimulatedMessageListModel;

class MessageSimulatorController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QUrl simulationFile READ simulationFile NOTIFY simulationFileChanged)
  Q_PROPERTY(SimulationState simulationState READ simulationState NOTIFY simulationStateChanged)
  Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
  Q_PROPERTY(bool simulationLooped READ isSimulationLooped WRITE setSimulationLooped NOTIFY simulationLoopedChanged)
  Q_PROPERTY(float messageFrequency READ messageFrequency WRITE setMessageFrequency NOTIFY messageFrequencyChanged)
  Q_PROPERTY(TimeUnit timeUnit READ timeUnit WRITE setTimeUnit NOTIFY timeUnitChanged)
  Q_PROPERTY(QAbstractListModel* messages READ messages NOTIFY messagesChanged)

public:
  enum class TimeUnit
  {
    Seconds = 0,
    Minutes = 1,
    Hours = 2
  };

  enum class SimulationState
  {
    Running = 0,
    Stopped = 1,
    Paused = 2
  };

  Q_ENUM(TimeUnit)
  Q_ENUM(SimulationState)

  explicit MessageSimulatorController(QObject* parent = nullptr);
  ~MessageSimulatorController();

  QUrl simulationFile() const;
  void setSimulationFile(const QUrl& simulationFile);

  void setPort(int port);
  int port() const;

  void setMessageFrequency(float messageFrequency);
  float messageFrequency() const;

  SimulationState simulationState() const;

  bool isSimulationLooped() const;
  void setSimulationLooped(bool simulationLooped);

  TimeUnit timeUnit() const;
  void setTimeUnit(TimeUnit timeUnit);

  QAbstractListModel* messages() const;

  Q_INVOKABLE void startSimulation(const QUrl& file);
  Q_INVOKABLE void pauseSimulation();
  Q_INVOKABLE void resumeSimulation();
  Q_INVOKABLE void stopSimulation();

  Q_INVOKABLE void sendMessage(const QString& message);

  Q_INVOKABLE static QString fromTimeUnit(TimeUnit timeUnit);
  Q_INVOKABLE static TimeUnit toTimeUnit(const QString& timeUnit);

signals:
  void simulationFileChanged();
  void simulationStateChanged();
  void portChanged();
  void simulationLoopedChanged();
  void messageFrequencyChanged();
  void timeUnitChanged();
  void messagesChanged();
  void errorOccurred(const QString& error);

private:
  Q_DISABLE_COPY(MessageSimulatorController)

  void saveSettings();
  void loadSettings();

  static float timeUnitToSeconds(TimeUnit timeUnit);

  Dsa::DataSender* m_dataSender = nullptr;
  AbstractMessageParser* m_messageParser = nullptr;
  SimulatedMessageListModel* m_messages = nullptr;

  QUdpSocket* m_udpSocket = nullptr;
  QTimer m_timer;

  QUrl m_simulationFile;

  int m_port = -1;
  float m_messageFrequency = 1;
  qint64 m_messagesSent = 0;

  bool m_simulationLooped = true;
  SimulationState m_simulationState = SimulationState::Stopped;

  TimeUnit m_timeUnit = TimeUnit::Seconds;
};

#endif // MESSAGESIMULATORCONTROLLER_H
