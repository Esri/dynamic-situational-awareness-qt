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

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "MessageSimulatorController.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

void printHelp()
{
  QTextStream out(stdout);
  out << "Available command line parameters:" << endl;
  out << "  -h                     Print help and exit" << endl;
  out << "  -c                     Console mode (no GUI)" << endl;
  out << "Parameters available only in console mode:" << endl;
  out << "  -p <port number>       Port number: Required" << endl;
  out << "  -f <filename>          Simulation file: Required" << endl;
  out << "  -q <frequency>         Frequency (messages per time unit); default is 1.0" << endl;
  out << "  -t <time unit>         Time unit for frequency; valid values are seconds," << endl <<
         "                         minute, and hour; default is second" << endl;
  out << "  -l                     Simulation loops through simulation file" << endl;
  out << "  -s                     Silent mode; no verbose output" << endl;
}

int main(int argc, char *argv[])
{
  QCoreApplication::setOrganizationName("Esri");
  QCoreApplication::setOrganizationDomain("esri.com");
  QCoreApplication::setApplicationName("MessageSimulator");

  bool isGui = true;
  QString simulationFile;
  int port = -1;
  float frequency = 1.0f;
  QString timeUnit = "second";
  bool isLoop = false;
  bool isVerbose = true;

  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-h"))
    {
#ifdef Q_OS_WIN
      AllocConsole() ;
      AttachConsole(GetCurrentProcessId());
      freopen("CON", "w", stdout);

      QCoreApplication app(argc, argv);
#endif

      // Print help
      printHelp();

#ifdef Q_OS_WIN
      return app.exec();
#else
      return 0;
#endif
    }
    else if (!strcmp(argv[i], "-c"))
    {
      isGui = false;
    }
    else if (!strcmp(argv[i], "-f"))
    {
      if ((i + 1) < argc)
      {
        simulationFile = QString(argv[++i]);
      }
    }
    else if (!strcmp(argv[i], "-p"))
    {
      if ((i + 1) < argc)
      {
        port = atoi(argv[++i]);
      }
    }
    else if (!strcmp(argv[i], "-q"))
    {
      if ((i + 1) < argc)
      {
        frequency = atof(argv[++i]);
      }
    }
    else if (!strcmp(argv[i], "-t"))
    {
      if ((i + 1) < argc)
      {
        timeUnit = QString(argv[++i]);
      }
    }
    else if (!strcmp(argv[i], "-l"))
    {
      isLoop = true;
    }
    else if (!strcmp(argv[i], "-s"))
    {
      isVerbose = false;
    }
  }

  if (!isGui)
  {
#ifdef Q_OS_WIN
    AllocConsole() ;
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
#endif

    if (simulationFile.isEmpty() || port == -1)
    {
      printHelp();
      return 0;
    }

    QCoreApplication app(argc, argv);

    MessageSimulatorController controller;

    if (isVerbose)
    {
      QObject::connect(&controller, &MessageSimulatorController::errorOccurred, &app, [](const QString& error)
      {
        qDebug() << error;
      });
    }

    controller.setMessageFrequency(frequency);
    controller.setTimeUnit(MessageSimulatorController::toTimeUnit(timeUnit));
    controller.setPort(port);
    controller.setSimulationLooped(isLoop);
    controller.startSimulation(QUrl::fromLocalFile(simulationFile));

    if (isVerbose)
    {
      QTextStream out(stdout);
      out << "Simulation started with file: " << controller.simulationFile().toString() << "\n";
      out << "UDP port: " << controller.port() << "\n";
      out << "Sending " << controller.messageFrequency() << " message per " <<
                  MessageSimulatorController::fromTimeUnit(controller.timeUnit()) << "\n";
      if (isLoop)
        out << "Simulation loop mode enabled\n";
    }

    return app.exec();
  }
  else
  {
    QGuiApplication app(argc, argv);

    qmlRegisterType<MessageSimulatorController>("Esri.MessageSimulator", 1, 0, "MessageSimulatorController");
    qRegisterMetaType<MessageSimulatorController::TimeUnit>("MessageSimulatorController::TimeUnit");
    qRegisterMetaType<MessageSimulatorController::SimulationState>("MessageSimulatorController::SimulationState");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
  }
}
