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

#include "ToolManager.h"
#include "ToolResourceProvider.h"
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
  return QStringLiteral("nav");
}

void NavigationController::setProperties(const QVariantMap& properties)
{
  Q_UNUSED(properties);
}

void NavigationController::updateGeoView()
{
  m_geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (m_geoView)
  {
    m_sceneView = dynamic_cast<SceneView*>(m_geoView);
    if (m_sceneView)
      m_is3d = true;
    else
      // set a mapView here when we have it.
      m_is3d = false;
  }
}

void NavigationController::zoomIn()
{
  zoom(2.0);
}

void NavigationController::zoomOut()
{
  zoom(0.5);
}

void NavigationController::tilt()
{
  if (m_is3d)
  {
    // get the current extent
    Envelope extent = m_sceneView->currentViewpoint(ViewpointType::CenterAndScale).targetGeometry().extent();
    // get the current camera
    Camera currentCamera = m_sceneView->currentViewpointCamera();

    if (currentCamera.isEmpty())
      return;

    // set the delta pitch.
    double deltaPitch = m_isCameraVertical ? 70.0 : -currentCamera.pitch();
    // rotate the camera using the delta pitch value
    Camera newCamera = currentCamera.rotateAround(extent.center(), 0, deltaPitch, 0);
    // set the sceneview to the new camera
    m_sceneView->setViewpointCamera(newCamera, 2.0);
    // switch the bool for next time
    m_isCameraVertical = !m_isCameraVertical;
    emit verticalChanged();
  }
}

void NavigationController::zoom(double zoomFactor)
{
  // get the current extent
  Envelope extent = m_sceneView->currentViewpoint(ViewpointType::CenterAndScale).targetGeometry().extent();
  // get the current camera
  Camera currentCamera = m_sceneView->currentViewpointCamera();

  if (currentCamera.isEmpty())
    return;

  // zoom in/out using the zoom factor
  Camera newCamera = currentCamera.zoomToward(extent.center(), zoomFactor);
  // set the sceneview to the new camera
  m_sceneView->setViewpointCamera(newCamera, 0.5);
}
