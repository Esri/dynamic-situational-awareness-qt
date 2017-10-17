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

#include "SceneQuickView.h"
#include "SceneView.h"
#include "Scene.h"
#include "GlobeCameraController.h"
#include "OrbitLocationCameraController.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "GeometryEngine.h"
#include <QDebug>

using namespace Esri::ArcGISRuntime;

NavigationController::NavigationController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &NavigationController::updateGeoView);

  updateGeoView();
}

NavigationController::~NavigationController()
{
}

QString NavigationController::toolName() const
{
  return QStringLiteral("NavigationController");
}

void NavigationController::updateGeoView()
{
  m_geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (m_geoView)
  {
    m_sceneView = dynamic_cast<SceneQuickView*>(m_geoView);
    if (m_sceneView)
    {
      m_is3d = true;

      connect(m_sceneView, &SceneQuickView::screenToLocationCompleted, this, [this](QUuid, Point location)
      {
        m_currentCenter = location;

        if (m_currentMode == Mode::Zoom)
        {
          zoom();
        }
        else if (m_currentMode == Mode::Rotate)
        {
          setRotationInternal();
        }
      });
    }
    else
    {
      // set a mapView here when we have it.
      m_is3d = false;
    }
  }
}

void NavigationController::zoomIn()
{
  if (!m_sceneView)
    return;

  m_currentMode = Mode::Zoom;
  m_sceneView->screenToLocation((m_sceneView->width()) * 0.5, (m_sceneView->height()) * 0.5);
}

void NavigationController::zoomOut()
{
  if (!m_sceneView)
    return;

  m_currentMode = Mode::Zoom;
  m_sceneView->screenToLocation(m_sceneView->width() * 0.5, m_sceneView->height() * 0.5);
}

//void NavigationController::tilt()
//{
//  if (m_is3d)
//  {
//    // get the current extent
//    Envelope extent = m_sceneView->currentViewpoint(ViewpointType::CenterAndScale).targetGeometry().extent();
//    // get the current camera
//    Camera currentCamera = m_sceneView->currentViewpointCamera();

//    if (currentCamera.isEmpty())
//      return;

//    // set the delta pitch.
//    double deltaPitch = m_isCameraVertical ? 70.0 : -currentCamera.pitch();
//    // rotate the camera using the delta pitch value
//    Camera newCamera = currentCamera.rotateAround(extent.center(), 0, deltaPitch, 0);
//    // set the sceneview to the new camera
//    m_sceneView->setViewpointCamera(newCamera, 2.0);
//    // switch the bool for next time
//    m_isCameraVertical = !m_isCameraVertical;
//    emit verticalChanged();
//  }
//}

void NavigationController::pan()
{
  if (!m_sceneView)
    return;

  GlobeCameraController* globeCameraController = new GlobeCameraController(this);
  m_sceneView->setCameraController(globeCameraController);
}

void NavigationController::setRotation()
{
  if (!m_sceneView)
    return;

  m_currentMode = Mode::Rotate;
  m_sceneView->screenToLocation(m_sceneView->width() * 0.5, m_sceneView->height() * 0.5);
}

void NavigationController::zoom()
{
  // get the current camera
  Camera currentCamera = m_sceneView->currentViewpointCamera();

  if (currentCamera.isEmpty())
    return;

  // zoom in/out using the zoom factor
  Camera newCamera = currentCamera.zoomToward(m_currentCenter, m_zoomFactor);
  // set the sceneview to the new camera
  m_sceneView->setViewpointCamera(newCamera, 0.5);
}

// getter for vertical
bool NavigationController::isVertical()
{
  return m_isCameraVertical;
}

// getter for zoom factor
double NavigationController::zoomFactor()
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

void NavigationController::setRotationInternal()
{
  // get the current camera
  Camera currentCamera = m_sceneView->currentViewpointCamera();
  GeodeticDistanceResult result = GeometryEngine::distanceGeodetic(currentCamera.location(), m_currentCenter, LinearUnit::meters(), m_geoView->spatialReference().unit(), GeodeticCurveType::Geodesic);
  double distance = result.distance();

  OrbitLocationCameraController* orbitController = new OrbitLocationCameraController(m_currentCenter, distance, this);
  orbitController->setCameraPitchOffset(currentCamera.pitch());
  orbitController->setCameraHeadingOffset(currentCamera.heading());
  m_sceneView->setCameraController(orbitController);
}
