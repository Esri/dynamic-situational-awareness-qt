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

#include "ToolManager.h"

using namespace Esri::ArcGISRuntime;

FollowPositionController::FollowPositionController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance()->addTool(this);
}

FollowPositionController::~FollowPositionController()
{
}

void FollowPositionController::init(GeoView* geoView)
{
  m_geoView = geoView;

  handleNewMode();
}

void FollowPositionController::nextMode()
{
  switch (m_mode) {
  case Mode::Disabled:
    m_mode = Mode::TrackUp;
    break;
  case Mode::TrackUp:
    m_mode = Mode::NorthUp;
    break;
  case Mode::NorthUp:
    m_mode = Mode::Disabled;
    break;
  default:
    m_mode = Mode::Disabled;
    break;
  }

  handleNewMode();
}

void FollowPositionController::setDisabled()
{
  m_mode = Mode::Disabled;

  handleNewMode();
}

bool FollowPositionController::isTrackUp() const
{
  return m_mode == Mode::TrackUp;
}


bool FollowPositionController::isNorthUp() const
{
  return m_mode == Mode::NorthUp;
}

QString FollowPositionController::toolName() const
{
  return QStringLiteral("follow position");
}

void FollowPositionController::handleNewMode()
{
  if (!m_geoView)
    return;

  if (handleFollowInMap())
    emit modeChanged();
  else if (handleFollowInScene())
    emit modeChanged();
}

bool FollowPositionController::handleFollowInMap()
{
  MapView* mapView = dynamic_cast<MapView*>(m_geoView);
  if (!mapView)
    return false;

  mapView->locationDisplay()->setAutoPanMode((isTrackUp() || isNorthUp()) ?
                                               LocationDisplayAutoPanMode::Navigation : LocationDisplayAutoPanMode::Off);
  return true;
}

bool FollowPositionController::handleFollowInScene()
{
  SceneView* sceneView = dynamic_cast<SceneView*>(m_geoView);
  if (!sceneView)
    return false;

  if (m_mode == Mode::Disabled)
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

    if (isNorthUp())
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
