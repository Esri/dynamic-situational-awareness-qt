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

#ifndef MESSAGESIMULATORCONTROLLER_H
#define MESSAGESIMULATORCONTROLLER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QUrl>
#include <QAbstractListModel>

class MessageSender;
class AbstractMessageParser;
class SimulatedMessageListModel;

class MessageSimulatorController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QUrl simulationFile READ simulationFile NOTIFY simulationFileChanged)
  Q_PROPERTY(bool simulationStarted READ isSimulationStarted NOTIFY simulationStartedChanged)
  Q_PROPERTY(bool simulationPaused READ isSimulationPaused NOTIFY simulationPausedChanged)
  Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
  Q_PROPERTY(bool simulationLooped READ isSimulationLooped WRITE setSimulationLooped NOTIFY simulationLoopedChanged)
  Q_PROPERTY(int messageFrequency READ messageFrequency WRITE setMessageFrequency NOTIFY messageFrequencyChanged)
  Q_PROPERTY(QString timeUnit READ timeUnit WRITE setTimeUnit NOTIFY timeUnitChanged)
  Q_PROPERTY(QAbstractListModel* messages READ messages NOTIFY messagesChanged)

public:
  explicit MessageSimulatorController(QObject* parent = nullptr);
  ~MessageSimulatorController();

  QUrl simulationFile() const;

  void setPort(int port);
  int port() const;

  void setMessageFrequency(int messageFrequency);
  int messageFrequency() const;

  bool isSimulationStarted() const;
  bool isSimulationPaused() const;

  bool isSimulationLooped() const;
  void setSimulationLooped(bool simulationLooped);

  QString timeUnit() const;
  void setTimeUnit(const QString& timeUnit);

  QAbstractListModel* messages() const;

  Q_INVOKABLE void startSimulation(const QUrl& file);
  Q_INVOKABLE void pauseSimulation();
  Q_INVOKABLE void resumeSimulation();
  Q_INVOKABLE void stopSimulation();

  Q_INVOKABLE void sendMessage(const QString& message);

signals:
  void simulationFileChanged();
  void simulationStartedChanged();
  void simulationPausedChanged();
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

  static float timeUnitToSeconds(const QString& timeUnit);

  MessageSender* m_messageSender = nullptr;
  AbstractMessageParser* m_messageParser = nullptr;
  SimulatedMessageListModel* m_messages = nullptr;

  QUdpSocket* m_udpSocket = nullptr;
  QTimer m_timer;

  QUrl m_simulationFile;

  int m_port = -1;
  int m_messageFrequency = 1;
  qint64 m_messagesSent = 0;

  bool m_simulationLooped = true;
  bool m_simulationStarted = false;
  bool m_simulationPaused = false;

  QString m_timeUnit = "second";
};

#endif // MESSAGESIMULATORCONTROLLER_H
