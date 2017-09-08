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

#ifndef VEHICLE_H
#define VEHICLE_H

namespace Esri
{
namespace ArcGISRuntime
{
class Error;
class Scene;
class SceneQuickView;
}
}

#include <QQuickItem>

class QScreen;

class Vehicle : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(double appScaleFactor READ appScaleFactor NOTIFY appScaleFactorChanged)

public:
  Vehicle(QQuickItem* parent = nullptr);
  ~Vehicle();

  void componentComplete() override;

signals:
  void appScaleFactorChanged();

private slots:
  void onError(const Esri::ArcGISRuntime::Error& error);
  void onScreenChanged(QScreen* screen);

private:
  double appScaleFactor() const;

  Esri::ArcGISRuntime::Scene*             m_scene = nullptr;
  Esri::ArcGISRuntime::SceneQuickView*    m_sceneView = nullptr;

  QString                                 m_dataPath;
  double                                  m_scaleFactor = 1.;
};

#endif // VEHICLE_H
