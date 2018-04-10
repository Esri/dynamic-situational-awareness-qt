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

#ifndef UTILITY_H
#define UTILITY_H

// C++ API headers
#include "Point.h"

// Qt headers
#include <QList>
#include <QObject>
#include <QString>
#include <QVector3D>

namespace Dsa {

class DsaUtility
{
public:

  static QString dataPath();
  static Esri::ArcGISRuntime::Point montereyCA();
  static double distance3D(const Esri::ArcGISRuntime::Point& from, const Esri::ArcGISRuntime::Point& to);
  static QVector3D toCartesianPoint(const Esri::ArcGISRuntime::Point& point);
};

} // Dsa

#endif // UTILITY_H
