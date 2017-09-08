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
#include "DsaUtility.h"

#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QStandardPaths>
#include <QWindow>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

using namespace Esri::ArcGISRuntime;

QList<QObject*> DsaUtility::tools; // placeholder until we have ToolManager

QString DsaUtility::dataPath()
{
  QDir dataDir;
#ifdef Q_OS_ANDROID
  dataDir = QDir(QStringLiteral("/sdcard"));
#elif defined Q_OS_IOS
  dataDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
#else
  dataDir = QDir::home();
#endif

  dataDir.cd("ArcGIS");
  dataDir.cd("Runtime");
  dataDir.cd("Data");
  dataDir.cd("DSA");

  return dataDir.exists() ? dataDir.absolutePath() : "";
}

Point DsaUtility::montereyCA()
{
  return Point(-121.9, 36.6, SpatialReference::wgs84());
}

double DsaUtility::scaleFactor(QScreen* screen /*= nullptr*/)
{
  if (screen == nullptr)
  {
    QGuiApplication* app = qobject_cast<QGuiApplication*>(QGuiApplication::instance());
    if (nullptr == app)
      return 1.;

    QWindow* focusWindow = app->focusWindow();
    screen = focusWindow ? focusWindow->screen() : app->primaryScreen();
  }

  if( nullptr == screen)
      return 1.;

  #ifdef Q_OS_WIN
    return scrn->logicalDotsPerInch() / USER_DEFAULT_SCREEN_DPI;
  #endif

  return screen->logicalDotsPerInch() / 72;
}
