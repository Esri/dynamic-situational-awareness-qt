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

#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

#include <QUrl>
#include <QScreen>
#include <QUuid>

#include "AbstractTool.h"
#include "Point.h"
#include "Camera.h"

namespace Esri {
namespace ArcGISRuntime
{
  class SceneView;
  class GeoView;
}}

class NavigationController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool vertical READ isVertical NOTIFY verticalChanged)
  Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)

public:

  explicit NavigationController(QObject* parent = nullptr);
  ~NavigationController();

  Q_INVOKABLE void zoomOut();
  Q_INVOKABLE void zoomIn();
  Q_INVOKABLE void pan();
  Q_INVOKABLE void setRotation();
  Q_INVOKABLE void set2D();

  QString toolName() const override;

  bool isVertical();
  double zoomFactor();
  void setZoomFactor(double value);

signals:
  void verticalChanged();
  void zoomFactorChanged();
  void screenToLocationCompleted(QUuid taskId, Esri::ArcGISRuntime::Point location);

private slots:
  void updateGeoView();

private:
  enum class Mode
  {
    Zoom,
    Rotate,
    Pan,
    Tilt
  };

  void center();
  void zoom();
  void setRotationInternal();
  void set2DInternal();
  double currentCameraDistance(const Esri::ArcGISRuntime::Camera &currentCamera);

  Esri::ArcGISRuntime::GeoView* m_geoView   = nullptr;
  Esri::ArcGISRuntime::SceneView* m_sceneView = nullptr;
  bool m_is3d                               = false;
  bool m_isCameraVertical                   = false;
  double m_zoomFactor                       = 1.0;
  Esri::ArcGISRuntime::Point m_currentCenter;
  Mode m_currentMode;
  bool m_enabled = false;
};



#endif // NAVIGATIONCONTROLLER_H
