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

// PCH header
#include "pch.hpp"

#include "NavigationController.h"

// C++ API headers
#include "Camera.h"
#include "ErrorException.h"
#include "GlobeCameraController.h"
#include "OrbitGeoElementCameraController.h"
#include "OrbitLocationCameraController.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "SceneView.h"
#include "SceneViewTypes.h"
#include "Viewpoint.h"

// Qt headers
#include <QFuture>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScreen>

// DSA headers
#include "DsaController.h"
#include "DsaUtility.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::NavigationController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for handling navigation for the app.
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
NavigationController::NavigationController(QObject* parent) :
  AbstractTool(parent)
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged, this, &NavigationController::updateGeoView);

  updateGeoView();

  ToolManager::instance().addTool(this);
}

/*!
  \brief Destructor.
 */
NavigationController::~NavigationController()
{
}

/*!
  \brief Returns the name of the tool - \c  "NavigationController".
 */
QString NavigationController::toolName() const
{
  return QStringLiteral("NavigationController");
}

/*!
  \internal
 */
void NavigationController::updateGeoView()
{
  m_geoView = ToolResourceProvider::instance()->geoView();
  if (!m_geoView)
    return;

  m_is3d = false;
  m_sceneView = dynamic_cast<SceneView*>(m_geoView);
  if (m_sceneView)
  {
    m_is3d = true;

    Scene* scene = m_sceneView->arcGISScene();
    if (!scene)
    {
      auto* sceneQV = static_cast<SceneQuickView*>(m_sceneView);
      connect(sceneQV, &SceneQuickView::viewpointChanged, this, [this]()
      {
        center();
      }, Qt::SingleShotConnection);
      return;
    }

    center();
  }
}

/*!
  \brief Zoom the app to the initial location.
 */
void NavigationController::zoomToInitialLocation()
{
  if (!m_is3d)
    return;

  if (const auto* dsaController = DsaController::instance(); dsaController)
  {
    if (const auto initialViewpoint = dsaController->readInitialLocation(); !initialViewpoint.isEmpty())
    {
      m_sceneView->setViewpointAsync(initialViewpoint, 1.0f);
      return;
    }
  }

  m_sceneView->setViewpointAsync(m_sceneView->arcGISScene()->initialViewpoint(), 1.0f);
}

/*!
  \brief Zoom in.
 */
void NavigationController::zoomIn()
{
  m_currentMode = Mode::Zoom;

  if (m_cameraMoveDistance < 0.0)
    m_cameraMoveDistance = -m_cameraMoveDistance;

  center();
}

/*!
  \brief Zoom out.
 */
void NavigationController::zoomOut()
{
  m_currentMode = Mode::Zoom;

  if (m_cameraMoveDistance > 0.0)
    m_cameraMoveDistance = -m_cameraMoveDistance;

  center();
}

/*!
  \brief Tilt the (3D) view to a 2D perspective.
 */
void NavigationController::set2D()
{
  m_currentMode = Mode::Tilt;
  center();
}

/*!
  \brief Switch the app to the default panning navogation mode.
 */
void NavigationController::pan()
{
  if (!m_sceneView)
    return;

  GlobeCameraController* globeCameraController = new GlobeCameraController(this);
  m_sceneView->setCameraController(globeCameraController);
}

/*!
  \brief Sets the app to rotation mode.
 */
void NavigationController::setRotation()
{
  m_currentMode = Mode::Rotate;
  center();
}

/*!
  \internal
 */
void NavigationController::zoom()
{
  // get the current camera
  Camera currentCamera = m_sceneView->currentViewpointCamera();

  if (currentCamera.isEmpty())
    return;

  // check if there is camera controller set
  if (m_sceneView->cameraController()->cameraControllerType() == CameraControllerType::OrbitGeoElementCameraController)
  {
    // get the controller
    OrbitGeoElementCameraController* controller = static_cast<OrbitGeoElementCameraController*>(m_sceneView->cameraController());
    // get the distance
    const double distance = controller->cameraDistance();
    // set the camera distance based on the zoom factor
    controller->setCameraDistance(distance / m_zoomFactor);
  }
  else
  {
    if (m_currentCenter.x() == 0 && m_currentCenter.y() == 0 && m_currentCenter.z() == 0)
    {
      Camera newCam = currentCamera.moveForward(m_cameraMoveDistance);
      m_sceneView->setViewpointCameraAsync(newCam);
    }
    else
    {
      // zoom in/out using the zoom factor
      Camera newCamera = currentCamera.zoomToward(m_currentCenter, m_zoomFactor);
      // set the sceneview to the new camera
      m_sceneView->setViewpointCameraAsync(newCamera, 0.5);
    }
  }
}

/*!
  \property NavigationController::vertical
  \brief Returns whether the camera is vertical.
 */
bool NavigationController::isVertical() const
{
  return m_isCameraVertical;
}

/*!
  \property NavigationController::zoomFactor
  \brief Returns the zoom factor.
 */
double NavigationController::zoomFactor() const
{
  return m_zoomFactor;
}

/*!
  \brief Sets the zoom factor to \a value.
 */
void NavigationController::setZoomFactor(double value)
{
  if (value == m_zoomFactor)
    return;

  m_zoomFactor = value;
  emit zoomFactorChanged();
}

/*!
  \property NavigationController::cameraMoveDistance
  \brief Returns the camera move distance in meters.
 */
double NavigationController::cameraMoveDistance() const
{
  return m_cameraMoveDistance;
}

/*!
  \brief Sets the camera move distance to \a value meters.
 */
void NavigationController::setCameraMoveDistance(double value)
{
  if (value == m_cameraMoveDistance)
    return;

  m_cameraMoveDistance = value;
  emit cameraMoveDistanceChanged();
}

/*!
  \internal
 */
void NavigationController::setRotationInternal()
{
  // get the current camera
  Camera currentCamera = m_sceneView->currentViewpointCamera();
  double distance = currentCameraDistance(currentCamera);

  OrbitLocationCameraController* orbitController = new OrbitLocationCameraController(m_currentCenter, distance, this);
  orbitController->setCameraPitchOffset(currentCamera.pitch());
  orbitController->setCameraHeadingOffset(currentCamera.heading());
  m_sceneView->setCameraController(orbitController);
}

/*!
  \internal
 */
void NavigationController::set2DInternal()
{
  static bool transitioningTo2D = false;
  if (transitioningTo2D)
    return;

  if (m_is3d)
  {
    // get the current camera
    const Camera currentCamera = m_sceneView->currentViewpointCamera();

    if (currentCamera.isEmpty() || currentCamera.pitch() == 0.0)
      return;

    // rotate the camera using the delta pitch value
    const Camera newCamera = currentCamera.rotateAround(m_currentCenter, 0., -currentCamera.pitch(), 0.);
    // set the sceneview to the new camera
    transitioningTo2D = true;
    m_sceneView->setViewpointCameraAsync(newCamera, 2.0).then(this, [](bool)
    {
      transitioningTo2D = false;
    }).onCanceled(this, []()
    {
      transitioningTo2D = false;
    }).onFailed(this, [](const ErrorException&)
    {
      transitioningTo2D = false;
    });
  }
}

/*!
  \internal
 */
void NavigationController::center()
{
  if (!m_sceneView)
    return;

  m_sceneView->screenToLocationAsync(m_sceneView->widthInPixels() * 0.5, m_sceneView->heightInPixels() * 0.5).then(this, [this](Point point)
  {
    // if the center of the screen does not intersect the globe an invalid point can be the result
    // don't update the class property if this happens, fallback is the previous center point
    if (!point.isEmpty() && point.isValid())
      m_currentCenter = point;

    // skip if center has never been initialized to a valid point
    if (m_currentCenter.isEmpty() || !m_currentCenter.isValid())
      return;

    if (m_currentMode == Mode::Zoom)
    {
      zoom();
    }
    else if (m_currentMode == Mode::Rotate)
    {
      setRotationInternal();
    }
    else if(m_currentMode == Mode::Tilt)
    {
      set2DInternal();
    }
  });
}

/*!
  \internal
 */
double NavigationController::currentCameraDistance(const Camera &currentCamera)
{
  if (currentCamera.isEmpty())
    return 0.0;

  return DsaUtility::distance3D(currentCamera.location(), m_currentCenter);
}

} // Dsa

// Signal Documentation
/*!
  \fn void NavigationController::verticalChanged();
  \brief Signal emitted when \l isVertical changes.
 */

/*!
  \fn void NavigationController::zoomFactorChanged();
  \brief Signal emitted when the zoom factor changes.
 */

/*!
  \fn void NavigationController::cameraMoveDistanceChanged();
  \brief Signal emitted when the camera move distance changes.
 */

