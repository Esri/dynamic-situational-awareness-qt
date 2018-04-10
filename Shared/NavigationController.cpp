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

#include "pch.hpp"

#include "NavigationController.h"

// example app headers
#include "DsaUtility.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "Camera.h"
#include "GeometryEngine.h"
#include "GlobeCameraController.h"
#include "Map.h"
#include "OrbitGeoElementCameraController.h"
#include "OrbitLocationCameraController.h"
#include "Scene.h"
#include "SceneView.h"

// Qt headers
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScreen>

using namespace Esri::ArcGISRuntime;

/*!
  \class NavigationController
  \inherits Toolkit::AbstractTool
  \brief Tool controller for handling navigation for the app.
 */

const QString NavigationController::INITIAL_LOCATION_PROPERTYNAME = "InitialLocation";

/*!
  \brief Constructor taking an optional \a parent.
 */
NavigationController::NavigationController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_initialCenter(DsaUtility::montereyCA())
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::sceneChanged, this, &NavigationController::setInitialLocation);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mapChanged, this, &NavigationController::setInitialLocation);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &NavigationController::updateGeoView);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::screenToLocationCompleted, this, &NavigationController::screenToLocationCompleted);

  updateGeoView();
  setInitialLocation();
}

/*!
  \brief Destructor.
 */
NavigationController::~NavigationController()
{
}

/*!
  \brief Returns the name of the tool.
 */
QString NavigationController::toolName() const
{
  return QStringLiteral("NavigationController");
}

/*!
 * \brief Sets any values in \a properties which are relevant for the navigation controller.
 *
 * This tool will use the following key/value pairs from the \a properties map if they are set:
 * \list
 *  \li InitialLocation. A JSON description of a the starting location for the app.
 * \endList
 */
void NavigationController::setProperties(const QVariantMap& properties)
{
  auto findIt = properties.constFind(NavigationController::INITIAL_LOCATION_PROPERTYNAME);
  if (findIt == properties.constEnd())
    return;

  const QVariant initialLocVar = findIt.value();
  if (initialLocVar.isNull())
    return;

  const QJsonObject initialLocation = QJsonObject::fromVariantMap(initialLocVar.toMap());
  if (initialLocation.isEmpty())
    return;

  bool propertiesChanged = false;

  // set the initial center Point from JSON if it is found
  auto centerIt = initialLocation.find("center");
  if (centerIt != initialLocation.constEnd())
  {
    const QJsonValue centerVal = centerIt.value();
    const QJsonDocument centerDoc = QJsonDocument(centerVal.toObject());
    const auto newCenter = Point::fromJson(centerDoc.toJson(QJsonDocument::JsonFormat::Compact));
    if (!m_initialCenter.equalsWithTolerance(newCenter, 0.0))
    {
      propertiesChanged = true;
      m_initialCenter = newCenter;
    }
  }

  // set the initial distance from JSON if it is found (if not default to the existing value)
  auto distanceIt = initialLocation.find("distance");
  if (distanceIt != initialLocation.constEnd())
  {
    const auto newDistance = distanceIt.value().toDouble(m_initialDistance);
    if (m_initialDistance != newDistance)
    {
      propertiesChanged = true;
      m_initialDistance = newDistance;
    }
  }

  // set the initial heading from JSON if it is found (if not default to the existing value)
  auto headingIt = initialLocation.find("heading");
  if (distanceIt != initialLocation.constEnd())
  {
    const auto newHeading = headingIt.value().toDouble(m_initialHeading);
    if (m_initialHeading != newHeading)
    {
      propertiesChanged = true;
      m_initialHeading = newHeading;
    }
  }

  // set the initial pitch from JSON if it is found (if not default to the existing value)
  auto pitchIt = initialLocation.find("pitch");
  if (pitchIt != initialLocation.constEnd())
  {
    const auto newPitch = pitchIt.value().toDouble(m_initialPitch);
    if (m_initialPitch != newPitch)
    {
      propertiesChanged = true;
      m_initialPitch = newPitch;
    }
  }

  // set the initial roll from JSON if it is found (if not default to the existing value)
  auto rollIt = initialLocation.find("roll");
  if (rollIt != initialLocation.constEnd())
  {
    const auto newRoll = rollIt.value().toDouble(m_initialRoll);
    if (m_initialRoll != newRoll)
    {
      propertiesChanged = true;
      m_initialRoll = newRoll;
    }
  }

  if (!propertiesChanged)
    return;

  setInitialLocation();
}

/*!
  \internal.
 */
void NavigationController::updateGeoView()
{
  m_geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!m_geoView)
    return;

  m_sceneView = dynamic_cast<SceneView*>(m_geoView);
  if (m_sceneView)
  {
    m_is3d = true;

    connect(this, &NavigationController::screenToLocationCompleted, this, [this](QUuid, Point location)
    {
      // check if called from the navigation controls
      if (!m_enabled)
        return;

      m_currentCenter = location;

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

      // reset
      m_enabled = false;
    });
  }
  else
  {
    // set the mapView here
    m_is3d = false;
  }
}

/*!
  \internal.
 */
void NavigationController::setInitialLocation()
{
  Scene* scene = Toolkit::ToolResourceProvider::instance()->scene();
  if (scene)
  {
    const Camera initCamera(m_initialCenter, m_initialDistance, m_initialHeading, m_initialPitch, m_initialRoll);
    Viewpoint initViewpoint(m_initialCenter, initCamera);
    scene->setInitialViewpoint(initViewpoint);

    return;
  }

  Map* map = Toolkit::ToolResourceProvider::instance()->map();
  if (!map)
    return;

  Viewpoint initViewpoint(m_initialCenter, 1000.0);
  map->setInitialViewpoint(initViewpoint);
}

/*!
  \brief Zoom the app to the initial location.
 */
void NavigationController::zoomToInitialLocation()
{
  Viewpoint initViewpoint;
  if (m_is3d)
  {
    const Camera initCamera(m_initialCenter, m_initialDistance, m_initialHeading, m_initialPitch, m_initialRoll);
    initViewpoint = Viewpoint(m_initialCenter, initCamera);
  }
  else
  {
    initViewpoint = Viewpoint(m_initialCenter, 1000.0);
  }

  m_geoView->setViewpoint(initViewpoint, 1.f);
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
  \internal.
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
      m_sceneView->setViewpointCamera(newCam);
    }
    else
    {
      // zoom in/out using the zoom factor
      Camera newCamera = currentCamera.zoomToward(m_currentCenter, m_zoomFactor);
      // set the sceneview to the new camera
      m_sceneView->setViewpointCamera(newCamera, 0.5);
    }
  }
}

/*!
  \brief Returns whether the camera is vertical.
 */
bool NavigationController::isVertical() const
{
  return m_isCameraVertical;
}

/*!
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
  \internal.
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
  \internal.
 */
void NavigationController::set2DInternal()
{
  if (m_is3d)
  {
    // get the current camera
    const Camera currentCamera = m_sceneView->currentViewpointCamera();

    if (currentCamera.isEmpty() || currentCamera.pitch() == 0.0)
      return;

    // rotate the camera using the delta pitch value
    const Camera newCamera = currentCamera.rotateAround(m_currentCenter, 0., -currentCamera.pitch(), 0.);
    // set the sceneview to the new camera
    m_sceneView->setViewpointCamera(newCamera, 2.0);
  }
}

/*!
  \internal.
 */
void NavigationController::center()
{
  if (!m_sceneView)
    return;

  m_enabled = true;

  m_sceneView->screenToLocation(m_sceneView->widthInPixels() * 0.5, m_sceneView->heightInPixels() * 0.5);
}

/*!
  \internal.
 */
double NavigationController::currentCameraDistance(const Camera &currentCamera)
{
  if (currentCamera.isEmpty())
    return 0.0;

  return DsaUtility::distance3D(currentCamera.location(), m_currentCenter);
}

/*!
  \brief Returns the initial roll in degrees.
 */
double NavigationController::initialRoll() const
{
  return m_initialRoll;
}

/*!
  \brief Sets the initial roll to \a initialRoll degrees.
 */
void NavigationController::setInitialRoll(double initialRoll)
{
  m_initialRoll = initialRoll;
  setInitialLocation();
}

/*!
  \brief Returns the initial pitch in degrees.
 */
double NavigationController::initialPitch() const
{
  return m_initialPitch;
}

/*!
  \brief Sets the initial pitch to \a initialPitch degrees.
 */
void NavigationController::setInitialPitch(double initialPitch)
{
  m_initialPitch = initialPitch;
  setInitialLocation();
}

/*!
  \brief Returns the initial heading in degrees.
 */
double NavigationController::initialHeading() const
{
  return m_initialHeading;
}

/*!
  \brief Sets the initial heading to \a initialHeading degrees.
 */
void NavigationController::setInitialHeading(double initialHeading)
{
  m_initialHeading = initialHeading;
  setInitialLocation();
}

/*!
  \brief Returns the initial distance in meters.
 */
double NavigationController::initialDistance() const
{
  return m_initialDistance;
}

/*!
  \brief Sets the initial distance to \a initialDistance meters.
 */
void NavigationController::setInitialDistance(double initialDistance)
{
  m_initialDistance = initialDistance;
  setInitialLocation();
}

/*!
  \brief Sets the initial center to \a initialCenter.
 */
void NavigationController::setInitialCenter(const Point& initialCenter)
{
  m_initialCenter = initialCenter;
  setInitialLocation();
}

/*!
  \brief Returns the initial center.
 */
Point NavigationController::initialCenter() const
{
  return m_initialCenter;
}
