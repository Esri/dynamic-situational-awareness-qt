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

#include "GlobeCameraController.h"
#include "MapView.h"
#include "OrbitGeoElementCameraController.h"
#include "SceneView.h"

#include "FollowPositionController.h"

#include "ToolResourceProvider.h"
#include "ToolManager.h"

using namespace Esri::ArcGISRuntime;

FollowPositionController::FollowPositionController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this,
          &FollowPositionController::updateGeoView);

  updateGeoView();
}

FollowPositionController::~FollowPositionController()
{
}

void FollowPositionController::init(GeoView* geoView)
{
  m_geoView = geoView;

  handleNewMode();
}

void FollowPositionController::setFollowMode(FollowPositionController::FollowMode followMode)
{
  if (m_mode == followMode)
    return;

  m_mode = followMode;
  handleNewMode();
}

FollowPositionController::FollowMode FollowPositionController::followMode() const
{
  return m_mode;
}

QString FollowPositionController::toolName() const
{
  return QStringLiteral("follow position");
}

void FollowPositionController::setFollowing(GeoElement* elementToFollow)
{
  if (elementToFollow == nullptr)
    return;

  SceneView* sceneView = dynamic_cast<SceneView*>(m_geoView);
  if (!sceneView)
    return;

  m_mode = FollowMode::TrackUp;

  OrbitGeoElementCameraController* followController = new OrbitGeoElementCameraController(elementToFollow, 2000., this);
  sceneView->setCameraController(followController);
}

void FollowPositionController::handleNewMode()
{
  if (!m_geoView)
    return;

  if (handleFollowInMap())
    emit followModeChanged();
  else if (handleFollowInScene())
    emit followModeChanged();
}

void FollowPositionController::updateGeoView()
{
  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (geoView)
    init(geoView);
}

bool FollowPositionController::handleFollowInMap()
{
  MapView* mapView = dynamic_cast<MapView*>(m_geoView);
  if (!mapView)
    return false;

  mapView->locationDisplay()->setAutoPanMode(m_mode == FollowMode::Disabled ?
                                               LocationDisplayAutoPanMode::Off :
                                               LocationDisplayAutoPanMode::Navigation );
  return true;
}

bool FollowPositionController::handleFollowInScene()
{
  SceneView* sceneView = dynamic_cast<SceneView*>(m_geoView);
  if (!sceneView)
    return false;

  if (m_mode == FollowMode::Disabled)
  {
    sceneView->setCameraController(new GlobeCameraController(this));
  }
  else
  {
    GraphicListModel* graphics = locationGraphicsModel();
    if (!graphics || graphics->rowCount() != 1)
      return true;

    Graphic* locationGraphic = graphics->at(0);
    if (!locationGraphic)
      return true;

    OrbitGeoElementCameraController* followController = new OrbitGeoElementCameraController(locationGraphic, 2000., this);

    if (m_mode == FollowMode::NorthUp)
    {
      followController->setAutoHeadingEnabled(false);
      followController->setCameraPitchOffset(0.);
    }

    sceneView->setCameraController(followController);
  }

  return true;
}

GraphicListModel* FollowPositionController::locationGraphicsModel() const
{
  if (!m_geoView)
    return nullptr;

  GraphicsOverlayListModel* overlays = m_geoView->graphicsOverlays();
  if (overlays->isEmpty())
    return nullptr;

  QString locationOverlayId("SCENEVIEWLOCATIONOVERLAY");
  for (int i = 0; i < overlays->rowCount(); ++i)
  {
    GraphicsOverlay* candidateOverlay = overlays->at(i);
    if (!candidateOverlay)
      continue;

    if (candidateOverlay->overlayId() != locationOverlayId)
      continue;

    return candidateOverlay->graphics();
  }

  return nullptr;
}
