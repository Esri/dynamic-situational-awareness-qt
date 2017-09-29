
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

#include <QSettings>
#include <QGuiApplication>
#include <QQuickView>
#include <QCommandLineParser>
#include <QDir>
#include <QMessageBox>
#include <QQmlEngine>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "ArcGISRuntimeEnvironment.h"

#include "SceneQuickView.h"

#include "BasemapPickerController.h"
#include "AddLocalDataController.h"
#include "FollowPositionController.h"
#include "LocationController.h"
#include "MessageFeedsController.h"
#include "TableOfContentsController.h"

#include "ArcGISRuntimeToolkit.h"
#include "ArcGISCompassController.h"

#include "AppInfo.h"
#include "Vehicle.h"

//------------------------------------------------------------------------------

#define kSettingsFormat                 QSettings::IniFormat

//------------------------------------------------------------------------------

#define kArgShowName                    "show"
#define kArgShowValueName               "showOption"
#define kArgShowDescription             "Show option maximized | minimized | fullscreen | normal | default"
#define kArgShowDefault                 "show"

#define kShowMaximized                  "maximized"
#define kShowMinimized                  "minimized"
#define kShowFullScreen                 "fullscreen"
#define kShowNormal                     "normal"
#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

//------------------------------------------------------------------------------

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);

  QCoreApplication::setApplicationName(kApplicationName);
  QCoreApplication::setApplicationVersion(kApplicationVersion);
  QCoreApplication::setOrganizationName(kOrganizationName);
#ifdef Q_OS_MAC
  QCoreApplication::setOrganizationDomain(kOrganizationName);
#else
  QCoreApplication::setOrganizationDomain(kOrganizationDomain);
#endif
  QSettings::setDefaultFormat(kSettingsFormat);

#ifdef Q_OS_WIN
  // Force usage of OpenGL ES through ANGLE on Windows
  QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

  // Before initializing ArcGIS Runtime, first set the
  // ArcGIS Runtime license setting required for your application.

  // ArcGISRuntimeEnvironment::setLicense("Place license string in here");

  //  use this code to check for initialization errors
  //  QObject::connect(ArcGISRuntimeEnvironment::instance(), &ArcGISRuntimeEnvironment::errorOccurred, [](const Error& error){
  //    QMessageBox msgBox;
  //    msgBox.setText(error.message);
  //    msgBox.exec();
  //  });

  //  if (ArcGISRuntimeEnvironment::initialize() == false)
  //  {
  //    application.quit();
  //    return 1;
  //  }

  // Register the map view for QML
  qmlRegisterType<SceneQuickView>("Esri.Vehicle", 1, 0, "SceneView");

  // Register the Vehicle (QQuickItem) for QML
  qmlRegisterType<Vehicle>("Esri.Vehicle", 1, 0, "Vehicle");
  qmlRegisterType<BasemapPickerController>("Esri.DSA", 1, 0, "BasemapPickerController");
  qmlRegisterType<AddLocalDataController>("Esri.DSA", 1, 0, "AddLocalDataController");
  qmlRegisterType<LocationController>("Esri.DSA", 1, 0, "LocationController");
  qmlRegisterType<MessageFeedsController>("Esri.DSA", 1, 0, "MessageFeedsController");
  qmlRegisterType<FollowPositionController>("Esri.DSA", 1, 0, "FollowPositionController");
  qmlRegisterType<TableOfContentsController>("Esri.DSA", 1, 0, "TableOfContentsController");

  // Register Toolkit Component Types
  Toolkit::ArcGISRuntimeToolkit::registerToolkitTypes();

  // Intialize application view
  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);

  // Add the import Path
  view.engine()->addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
  QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);
  QString arcGISToolkitImportPath = QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
  // on some linux platforms the string 'linux' is replaced with 1
  // fix the replacement paths which were created
  QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
  arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
  arcGISToolkitImportPath = arcGISToolkitImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif

  // Add the Runtime and Extras path
  view.engine()->addImportPath(arcGISRuntimeImportPath);
  // Add the Toolkit path
  view.engine()->addImportPath(arcGISToolkitImportPath);

  // Set the source
  view.setSource(QUrl(kApplicationSourceUrl));

#if !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)
  // Process command line
  QCommandLineOption showOption(kArgShowName, kArgShowDescription, kArgShowValueName, kArgShowDefault);

  QCommandLineParser commandLineParser;

  commandLineParser.setApplicationDescription(kApplicationDescription);
  commandLineParser.addOption(showOption);
  commandLineParser.addHelpOption();
  commandLineParser.addVersionOption();
  commandLineParser.process(app);

  // Show app window

  auto showValue = commandLineParser.value(kArgShowName).toLower();

  if (showValue.compare(kShowMaximized) == 0)
  {
    view.showMaximized();
  }
  else if (showValue.compare(kShowMinimized) == 0)
  {
    view.showMinimized();
  }
  else if (showValue.compare(kShowFullScreen) == 0)
  {
    view.showFullScreen();
  }
  else if (showValue.compare(kShowNormal) == 0)
  {
    view.showNormal();
  }
  else
  {
    view.show();
  }

#else
  view.show();
#endif

  return app.exec();
}

//------------------------------------------------------------------------------
