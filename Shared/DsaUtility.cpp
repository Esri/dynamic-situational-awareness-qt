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

#include "DsaUtility.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QScreen>
#include <QGuiApplication>

using namespace Esri::ArcGISRuntime;

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
