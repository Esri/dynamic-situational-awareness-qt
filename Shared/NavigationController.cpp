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

#include "NavigationController.h"

#include "Map.h"
#include "SceneView.h"
#include "Scene.h"
#include "GlobeCameraController.h"
#include "OrbitLocationCameraController.h"
#include "OrbitGeoElementCameraController.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "GeometryEngine.h"
#include "DsaUtility.h"

#include <QScreen>
#include <QDebug>
#include <QGuiApplication>

using namespace Esri::ArcGISRuntime;

const QString NavigationController::INITIAL_LOCATION_PROPERTYNAME = "InitialLocation";

NavigationController::NavigationController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_initialLocation(DsaUtility::montereyCA())
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::sceneChanged, this, &NavigationController::setInitialLocation);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mapChanged, this, &NavigationController::setInitialLocation);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &NavigationController::updateGeoView);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::screenToLocationCompleted, this, &NavigationController::screenToLocationCompleted);

  updateGeoView();
  setInitialLocation();
}

NavigationController::~NavigationController()
{
}

QString NavigationController::toolName() const
{
  return QStringLiteral("NavigationController");
}

void NavigationController::setProperties(const QVariantMap &properties)
{
  const QStringList initialLocation = properties.value(NavigationController::INITIAL_LOCATION_PROPERTYNAME).toStringList();
  if (initialLocation.length() <= 5)
    return;

  const double x = QString(initialLocation.at(0)).toDouble();
  const double y = QString(initialLocation.at(1)).toDouble();
  m_initialLocation = Point(x, y, SpatialReference::wgs84());
  m_initialDistance = QString(initialLocation.at(2)).toDouble();
  m_initialHeading = QString(initialLocation.at(3)).toDouble();
  m_initialPitch = QString(initialLocation.at(4)).toDouble();
  m_initialRoll = QString(initialLocation.at(5)).toDouble();

  setInitialLocation();
}

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

void NavigationController::setInitialLocation()
{
  Scene* scene = Toolkit::ToolResourceProvider::instance()->scene();
  if (scene)
  {
    const Camera initCamera(m_initialLocation, m_initialDistance, m_initialHeading, m_initialPitch, m_initialRoll);
    Viewpoint initViewpoint(m_initialLocation, initCamera);
    scene->setInitialViewpoint(initViewpoint);

    return;
  }

  Map* map = Toolkit::ToolResourceProvider::instance()->map();
  if (!map)
    return;

  Viewpoint initViewpoint(m_initialLocation, 1000.0);
  map->setInitialViewpoint(initViewpoint);
}

void NavigationController::zoomToInitialLocation()
{
  Viewpoint initViewpoint;
  if (m_is3d)
  {
    const Camera initCamera(m_initialLocation, m_initialDistance, m_initialHeading, m_initialPitch, m_initialRoll);
    initViewpoint = Viewpoint(m_initialLocation, initCamera);
  }
  else
  {
    initViewpoint = Viewpoint(m_initialLocation, 1000.0);
  }

  m_geoView->setViewpoint(initViewpoint, 1.f);
}

void NavigationController::zoomIn()
{
  m_currentMode = Mode::Zoom;

  if (m_cameraMoveDistance < 0.0)
    m_cameraMoveDistance = -m_cameraMoveDistance;

  center();
}

void NavigationController::zoomOut()
{
  m_currentMode = Mode::Zoom;

  if (m_cameraMoveDistance > 0.0)
    m_cameraMoveDistance = -m_cameraMoveDistance;

  center();
}

void NavigationController::set2D()
{
  m_currentMode = Mode::Tilt;
  center();
}


void NavigationController::pan()
{
  if (!m_sceneView)
    return;

  GlobeCameraController* globeCameraController = new GlobeCameraController(this);
  m_sceneView->setCameraController(globeCameraController);
}

void NavigationController::setRotation()
{
  m_currentMode = Mode::Rotate;
  center();
}


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

// getter for vertical
bool NavigationController::isVertical() const
{
  return m_isCameraVertical;
}

// getter for zoom factor
double NavigationController::zoomFactor() const
{
  return m_zoomFactor;
}

// setter for zooom factor
void NavigationController::setZoomFactor(double value)
{
  if (value == m_zoomFactor)
    return;

  m_zoomFactor = value;
  emit zoomFactorChanged();
}

// getter for zoom factor
double NavigationController::cameraMoveDistance() const
{
  return m_cameraMoveDistance;
}

// setter for zooom factor
void NavigationController::setCameraMoveDistance(double value)
{
  if (value == m_cameraMoveDistance)
    return;

  m_cameraMoveDistance = value;
  emit cameraMoveDistanceChanged();
}

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

void NavigationController::center()
{
  if (!m_sceneView)
    return;

  m_enabled = true;
  const double factor = DsaUtility::dipsToPixels();
  m_sceneView->screenToLocation(static_cast<int>(m_sceneView->sceneWidth() / factor) * 0.5, static_cast<int>(m_sceneView->sceneHeight() / factor) * 0.5);
}

double NavigationController::currentCameraDistance(const Camera &currentCamera)
{
  if (currentCamera.isEmpty())
    return 0.0;

  const GeodeticDistanceResult result = GeometryEngine::distanceGeodetic(currentCamera.location(), m_currentCenter, LinearUnit::meters(), m_geoView->spatialReference().unit(), GeodeticCurveType::Geodesic);
  return result.distance();
}
