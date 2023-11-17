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

#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

// Qt headers
#include <QUuid>

// C++ API headers
#include "Point.h"

// dsa headers
#include "AbstractTool.h"

namespace Esri::ArcGISRuntime {
  class Camera;
  class GeoView;
  class SceneView;
}

namespace Dsa {

class NavigationController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool vertical READ isVertical NOTIFY verticalChanged)
  Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
  Q_PROPERTY(double cameraMoveDistance READ cameraMoveDistance WRITE setCameraMoveDistance NOTIFY cameraMoveDistanceChanged)

public:

  explicit NavigationController(QObject* parent = nullptr);
  ~NavigationController();

  Q_INVOKABLE void zoomToInitialLocation();
  Q_INVOKABLE void zoomOut();
  Q_INVOKABLE void zoomIn();
  Q_INVOKABLE void pan();
  Q_INVOKABLE void setRotation();
  Q_INVOKABLE void set2D();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  bool isVertical() const;
  double zoomFactor() const;
  void setZoomFactor(double value);
  double cameraMoveDistance() const;
  void setCameraMoveDistance(double value);

signals:
  void verticalChanged();
  void zoomFactorChanged();
  void screenToLocationCompleted(QUuid taskId, Esri::ArcGISRuntime::Point location);
  void cameraMoveDistanceChanged();

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

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  Esri::ArcGISRuntime::SceneView* m_sceneView = nullptr;
  bool m_is3d = false;
  bool m_isCameraVertical = false;
  double m_zoomFactor = 1.0;
  Esri::ArcGISRuntime::Point m_currentCenter;
  Mode m_currentMode;
  bool m_enabled = false;
  bool m_isZoomIn = false;
  double m_cameraMoveDistance = 1000.0;
};

} // Dsa

#endif // NAVIGATIONCONTROLLER_H
