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

#include "ViewshedController.h"

// PCH header
#include "pch.hpp"

// example app headers
#include "DsaUtility.h"
#include "GeoElementViewshed360.h"
#include "GraphicsOverlaysResultsManager.h"
#include "LocationController.h"
#include "LocationDisplay3d.h"
#include "LocationViewshed360.h"
#include "ViewshedListModel.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "GeoElementViewshed.h"
#include "GlobeCameraController.h"
#include "LocationViewshed.h"
#include "OrbitLocationCameraController.h"
#include "SceneQuickView.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleRenderer.h"

// STL headers
#include <cmath>

using namespace Esri::ArcGISRuntime;

const QString ViewshedController::VIEWSHED_HEADING_ATTRIBUTE = QStringLiteral("heading");
const QString ViewshedController::VIEWSHED_PITCH_ATTRIBUTE = QStringLiteral("pitch");

static int s_viewshedCount = 0;

constexpr double c_defaultOffsetZ = 5.0;
constexpr double c_defaultConeWidth = 12.0;
constexpr double c_defaultIdentifyTolerance = 5.0;

ViewshedController::ViewshedController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_analysisOverlay(new AnalysisOverlay(this)),
  m_viewsheds(new ViewshedListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]
  {
    setSceneView(dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView()));
  });

  connectMouseSignals();

  auto sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (sceneView)
    setSceneView(sceneView);

  connect(m_viewsheds, &ViewshedListModel::viewshedRemoved, this, [this](Viewshed360* viewshed)
  {
    std::unique_ptr<Viewshed360> viewshedPtr(viewshed);

    // remove viewshed from analysis overlay and delete object
    viewshedPtr->removeFromOverlay();

    if (viewshed == m_locationDisplayViewshed)
    {
      m_locationDisplayViewshed = nullptr;
    }
  });
}

ViewshedController::~ViewshedController()
{
}

void ViewshedController::setSceneView(SceneView* sceneView)
{
  if (!sceneView)
    return;

  m_sceneView = sceneView;

  if (!m_sceneView->analysisOverlays()->contains(m_analysisOverlay))
    m_sceneView->analysisOverlays()->append(m_analysisOverlay);
}

void ViewshedController::connectMouseSignals()
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked, this, &ViewshedController::onMouseClicked);
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseMoved, this, &ViewshedController::onMouseMoved);
}

void ViewshedController::onMouseClicked(QMouseEvent& event)
{
  if (!isActive() || !m_sceneView)
    return;

  switch (m_activeMode)
  {
  case AddLocationViewshed360:
  {
    const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
    addLocationViewshed360(pt);
    break;
  }
  case AddGeoElementViewshed360:
  {
    if (!m_identifyConn)
    {
      m_identifyConn = connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyGraphicsOverlaysCompleted,
                               this, [this](const QUuid& taskId, const QList<IdentifyGraphicsOverlayResult*>& identifyResults)
      {
        if (taskId != m_identifyTaskWatcher.taskId())
          return;

        m_identifyTaskWatcher = TaskWatcher();

        // Create a RAII helper to ensure we clean up the results
        GraphicsOverlaysResultsManager resultsManager(identifyResults);

        if (!isActive() || resultsManager.m_results.isEmpty() || resultsManager.m_results[0]->graphics().isEmpty())
        {
          return;
        }

        auto graphic = resultsManager.m_results[0]->graphics()[0];
        graphic->setParent(nullptr);
        addGeoElementViewshed360(graphic);
      });
    }

    m_identifyTaskWatcher = m_sceneView->identifyGraphicsOverlays(event.x(), event.y(), c_defaultIdentifyTolerance, false, 1);
    break;
  }
  default:
    break;
  }
}

void ViewshedController::onMouseMoved(QMouseEvent& event)
{
  if (!isActive() || !m_sceneView)
    return;

  if (m_activeMode != ViewshedActiveMode::AddLocationViewshed360)
    return;

  if (!m_activeViewshed)
    return;

  auto locViewshed = dynamic_cast<LocationViewshed360*>(m_activeViewshed);
  if (!locViewshed)
    return;

  const Point point = m_sceneView->screenToBaseSurface(event.x(), event.y());
  locViewshed->setPoint(point);

  event.accept();
}

void ViewshedController::addLocationDisplayViewshed()
{
  if (m_locationDisplayViewshed)
    return;

  LocationController* locationController = Toolkit::ToolManager::instance().tool<LocationController>();
  if (!locationController)
    return;

  Graphic* locationGraphic = locationController->locationDisplay()->locationGraphic();
  m_locationDisplayViewshed = new GeoElementViewshed360(locationGraphic, m_analysisOverlay, VIEWSHED_HEADING_ATTRIBUTE, VIEWSHED_PITCH_ATTRIBUTE, this);
  m_locationDisplayViewshed->setName(QStringLiteral("Location Display Viewshed"));
  m_locationDisplayViewshed->setOffsetZ(c_defaultOffsetZ);
  m_analysisOverlay->analyses()->append(m_locationDisplayViewshed->viewshed());
  m_viewsheds->append(m_locationDisplayViewshed);

  setActiveViewshedIndex(m_viewsheds->rowCount() - 1);
}

void ViewshedController::addLocationViewshed360(const Esri::ArcGISRuntime::Point& point)
{
  if (!m_graphicsOverlay)
  {
    if (!m_sceneView)
      return;

    m_graphicsOverlay = new GraphicsOverlay(this);
    m_graphicsOverlay->setOverlayId(QStringLiteral("LocationViewshed360 graphics overlay"));
    SimpleMarkerSceneSymbol* smss = SimpleMarkerSceneSymbol::cone(QColor("red"), 16, 32.0, this);
    smss->setAnchorPosition(SceneSymbolAnchorPosition::Bottom);
    SimpleRenderer* renderer = new SimpleRenderer(smss, this);
    const QString headingExpression = QString("[%1]").arg(VIEWSHED_HEADING_ATTRIBUTE);
    const QString pitchExpression = QString("[%1]").arg(VIEWSHED_PITCH_ATTRIBUTE);
    const QString rollExpression(QStringLiteral("[roll]"));
    renderer->setSceneProperties(RendererSceneProperties(headingExpression, pitchExpression, rollExpression));
    m_graphicsOverlay->setRenderer(renderer);
    m_sceneView->graphicsOverlays()->append(m_graphicsOverlay);
  }

  if (m_activeViewshed)
  {
    auto locViewshed = dynamic_cast<LocationViewshed360*>(m_activeViewshed);
    if (locViewshed != nullptr)
    {
      locViewshed->setPoint(point);
      return;
    }
  }

  auto locationViewshed360 = new LocationViewshed360(point, m_graphicsOverlay, m_analysisOverlay, this);
  s_viewshedCount++;
  locationViewshed360->setName(QString("Viewshed %1").arg(QString::number(s_viewshedCount)));
  m_analysisOverlay->analyses()->append(locationViewshed360->viewshed());
  m_viewsheds->append(locationViewshed360);

  if (m_followCamCtrllr)
  {
    delete m_followCamCtrllr;
    m_followCamCtrllr = nullptr;
  }

  if (m_sceneView)
  {
    const Camera currCam = m_sceneView->currentViewpointCamera();
    const double currDistance = DsaUtility::distance3D(currCam.location(), currCam.location());
    m_followCamCtrllr = new OrbitLocationCameraController(currCam.location(), currDistance, this);
    m_followCamCtrllr->setCameraPitchOffset(currCam.pitch());
    m_followCamCtrllr->setCameraHeadingOffset(currCam.heading());
    m_followCamCtrllr->setCameraHeadingOffsetInteractive(false);
    m_followCamCtrllr->setCameraPitchOffsetInteractive(false);

    m_sceneView->setCameraController(m_followCamCtrllr);
  }

  setActiveViewshedIndex(m_viewsheds->rowCount() - 1);
}

void ViewshedController::addGeoElementViewshed360(GeoElement* geoElement)
{
  removeActiveViewshed();

  auto geoElementViewshed360 = new GeoElementViewshed360(geoElement, m_analysisOverlay, QString(), QString(), this);
  s_viewshedCount++;
  geoElementViewshed360->setName(QString("Viewshed %1").arg(QString::number(s_viewshedCount)));
  if (!geoElement->parent())
    geoElement->setParent(geoElementViewshed360);

  geoElementViewshed360->setOffsetZ(c_defaultOffsetZ);
  m_analysisOverlay->analyses()->append(geoElementViewshed360->viewshed());
  m_viewsheds->append(geoElementViewshed360);

  setActiveViewshedIndex(m_viewsheds->rowCount() - 1);
}

bool ViewshedController::isLocationDisplayViewshedActive() const
{
  return m_locationDisplayViewshed != nullptr;
}

ViewshedController::ViewshedActiveMode ViewshedController::activeMode() const
{
  return m_activeMode;
}

void ViewshedController::setActiveMode(ViewshedActiveMode mode)
{
  if (m_activeMode == mode)
    return;

  removeActiveViewshed();

  // re-enable view panning
  if (m_sceneView && m_activeMode == ViewshedActiveMode::AddLocationViewshed360)
  {
    if (!m_navCamCtrllr)
      m_navCamCtrllr = new GlobeCameraController(this);

    m_sceneView->setCameraController(m_navCamCtrllr);

    if (m_followCamCtrllr)
    {
      delete m_followCamCtrllr;
      m_followCamCtrllr = nullptr;
    }
  }

  m_activeMode = mode;

  if (m_activeMode == ViewshedActiveMode::AddMyLocationViewshed360)
    addLocationDisplayViewshed();

  emit activeModeChanged();
}

QAbstractListModel* ViewshedController::viewsheds() const
{
  return m_viewsheds;
}

QString ViewshedController::toolName() const
{
  return QStringLiteral("viewshed");
}

// active viewshed methods
Viewshed360* ViewshedController::activeViewshed() const
{
  return m_activeViewshed;
}

void ViewshedController::removeActiveViewshed()
{
  if (!m_activeViewshed)
    return;

  m_viewsheds->removeOne(m_activeViewshed);

  if (m_viewsheds->isEmpty())
    setActiveViewshedIndex(-1);
  else if (m_activeViewshedIndex >= m_viewsheds->rowCount())
    setActiveViewshedIndex(m_activeViewshedIndex - 1);
  else
    updateActiveViewshed();
}

void ViewshedController::finishActiveViewshed()
{
  setActiveViewshedIndex(-1);
}

bool ViewshedController::isActiveViewshedEnabled() const
{
  return m_activeViewshed != nullptr;
}

int ViewshedController::activeViewshedIndex() const
{
  return m_activeViewshedIndex;
}

void ViewshedController::setActiveViewshedIndex(int index)
{
  if (m_activeViewshedIndex == index)
    return;

  m_activeViewshedIndex = index;

  updateActiveViewshed();
}

double ViewshedController::activeViewshedMinDistance() const
{
  return m_activeViewshed ? m_activeViewshed->minDistance() : NAN;
}

void ViewshedController::setActiveViewshedMinDistance(double minDistance)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->setMinDistance(minDistance);
}

double ViewshedController::activeViewshedMaxDistance() const
{
  return m_activeViewshed ? m_activeViewshed->maxDistance() : NAN;
}

void ViewshedController::setActiveViewshedMaxDistance(double maxDistance)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->setMaxDistance(maxDistance);
}

double ViewshedController::activeViewshedHorizontalAngle() const
{
  return m_activeViewshed ? m_activeViewshed->horizontalAngle() : NAN;
}

void ViewshedController::setActiveViewshedHorizontalAngle(double horizontalAngle)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->horizontalAngle() == horizontalAngle)
    return;

  m_activeViewshed->setHorizontalAngle(horizontalAngle);
}

double ViewshedController::activeViewshedVerticalAngle() const
{
  return m_activeViewshed ? m_activeViewshed->verticalAngle() : NAN;
}

void ViewshedController::setActiveViewshedVerticalAngle(double verticalAngle)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->verticalAngle() == verticalAngle)
    return;

  m_activeViewshed->setVerticalAngle(verticalAngle);
}

double ViewshedController::activeViewshedHeading() const
{
  return m_activeViewshed ? m_activeViewshed->heading() : NAN;
}

void ViewshedController::setActiveViewshedHeading(double heading)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->heading() == heading)
    return;

  m_activeViewshed->setHeading(heading);
}

double ViewshedController::activeViewshedPitch() const
{
  return m_activeViewshed ? m_activeViewshed->pitch() : NAN;
}

void ViewshedController::setActiveViewshedPitch(double pitch)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->pitch() == pitch)
    return;

  m_activeViewshed->setPitch(pitch);
}

double ViewshedController::activeViewshedMinPitch() const
{
  if (!m_activeViewshed)
    return NAN;

  constexpr double minPitchLocationViewshed = 0.0;
  constexpr double minPitchGeoElementViewshed = -90.0;

  if (dynamic_cast<LocationViewshed360*>(m_activeViewshed))
    return minPitchLocationViewshed;

  return minPitchGeoElementViewshed;
}

double ViewshedController::activeViewshedMaxPitch() const
{
  if (!m_activeViewshed)
    return NAN;

  constexpr double maxPitchLocationViewshed = 179.0;
  constexpr double maxPitchGeoElementViewshed = 90.0;

  if (dynamic_cast<LocationViewshed360*>(m_activeViewshed))
    return maxPitchLocationViewshed;

  return maxPitchGeoElementViewshed;
}

double ViewshedController::activeViewshedOffsetZ() const
{
  constexpr double offsetZDefault = 0.0;
  return m_activeViewshed ? m_activeViewshed->offsetZ() : offsetZDefault;
}

void ViewshedController::setActiveViewshedOffsetZ(double offsetZ)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->setOffsetZ(offsetZ);
}

bool ViewshedController::isActiveViewshedHeadingEnabled() const
{
  return m_activeViewshed ? m_activeViewshed->isHeadingEnabled() : false;
}

bool ViewshedController::isActiveViewshedPitchEnabled() const
{
  return m_activeViewshed ? m_activeViewshed->isPitchEnabled() : false;
}

bool ViewshedController::isActiveViewshedOffsetZEnabled() const
{
  return m_activeViewshed ? m_activeViewshed->isOffsetZEnabled() : false;
}

bool ViewshedController::isActiveViewshed360Mode() const
{
  return m_activeViewshed ? m_activeViewshed->is360Mode() : true;
}

void ViewshedController::setActiveViewshed360Mode(bool is360Mode)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->set360Mode(is360Mode);
}

void ViewshedController::updateActiveViewshedSignals()
{
  disconnectActiveViewshedSignals();

  if (!m_activeViewshed)
    return;

  // connect to Viewshed360 signals
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::minDistanceChanged, this, &ViewshedController::activeViewshedMinDistanceChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::maxDistanceChanged, this, &ViewshedController::activeViewshedMaxDistanceChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::horizontalAngleChanged, this, &ViewshedController::activeViewshedHorizontalAngleChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::verticalAngleChanged, this, &ViewshedController::activeViewshedVerticalAngleChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::headingChanged, this, &ViewshedController::activeViewshedHeadingChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::pitchChanged, this, &ViewshedController::activeViewshedPitchChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::offsetZChanged, this, &ViewshedController::activeViewshedOffsetZChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::headingEnabledChanged, this, &ViewshedController::activeViewshedHeadingEnabledChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::pitchEnabledChanged, this, &ViewshedController::activeViewshedPitchEnabledChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::offsetZEnabledChanged, this, &ViewshedController::activeViewshedOffsetZEnabledChanged);
  m_activeViewshedConns << connect(m_activeViewshed, &Viewshed360::is360ModeChanged, this, &ViewshedController::activeViewshed360ModeChanged);
}

void ViewshedController::updateActiveViewshed()
{
  if (m_activeViewshedIndex == -1)
  {
    disconnectActiveViewshedSignals();
    m_activeViewshed = nullptr;
    emitActiveViewshedSignals();
    return;
  }

  m_activeViewshed = m_viewsheds->at(m_activeViewshedIndex);

  updateActiveViewshedSignals();
  emitActiveViewshedSignals();
}

void ViewshedController::disconnectActiveViewshedSignals()
{
  if (!m_activeViewshedConns.isEmpty())
  {
    for (auto conn : m_activeViewshedConns)
    {
      disconnect(conn);
    }

    m_activeViewshedConns.clear();
  }
}

void ViewshedController::emitActiveViewshedSignals()
{
  emit activeViewshedEnabledChanged();
  emit activeViewshedMinDistanceChanged();
  emit activeViewshedMaxDistanceChanged();
  emit activeViewshedHorizontalAngleChanged();
  emit activeViewshedVerticalAngleChanged();
  emit activeViewshedHeadingChanged();
  emit activeViewshedPitchChanged();
  emit activeViewshedMinPitchChanged();
  emit activeViewshedMaxPitchChanged();
  emit activeViewshedOffsetZChanged();
  emit activeViewshedHeadingEnabledChanged();
  emit activeViewshedPitchEnabledChanged();
  emit activeViewshedOffsetZEnabledChanged();
  emit activeViewshed360ModeChanged();
}
