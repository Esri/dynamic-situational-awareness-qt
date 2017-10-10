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

#include "AbstractTool.h"
#include "Point.h"

namespace Esri {
namespace ArcGISRuntime
{
  class SceneQuickView;
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

  QString toolName() const override;

  bool isVertical();
  double zoomFactor();
  void setZoomFactor(double value);

signals:
  void verticalChanged();
  void zoomFactorChanged();

private slots:
  void updateGeoView();

private:
  enum class Mode
  {
    Zoom,
    Rotate,
    Pan
  };

  void zoom();
  void setRotationInternal();

  Esri::ArcGISRuntime::GeoView* m_geoView   = nullptr;
  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  bool m_is3d                               = false;
  bool m_isCameraVertical                   = false;
  double m_zoomFactor                       = 1.0;
  Esri::ArcGISRuntime::Point m_currentCenter;
  Mode m_currentMode;
};



#endif // NAVIGATIONCONTROLLER_H
