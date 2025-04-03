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

#include "ViewshedController.h"

// C++ API headers
#include "AnalysisListModel.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "Camera.h"
#include "DynamicEntity.h"
#include "DynamicEntityObservation.h"
#include "GlobeCameraController.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "GraphicsOverlayListModel.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "IdentifyLayerResult.h"
#include "OrbitLocationCameraController.h"
#include "RendererSceneProperties.h"
#include "SimpleMarkerSceneSymbol.h"
#include "SimpleRenderer.h"
#include "SymbolTypes.h"
#include "Viewshed.h"

// Qt headers
#include <QFuture>

// STL headers
#include <cmath>

// DSA headers
#include "DsaUtility.h"
#include "GeoElementUtils.h"
#include "GeoElementViewshed360.h"
#include "GraphicsOverlaysResultsManager.h"
#include "IdentifyController.h"
#include "LayerResultsManager.h"
#include "LocationController.h"
#include "LocationDisplay3d.h"
#include "LocationViewshed360.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "ViewshedListModel.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString ViewshedController::VIEWSHED_HEADING_ATTRIBUTE = QStringLiteral("heading");
const QString ViewshedController::VIEWSHED_PITCH_ATTRIBUTE = QStringLiteral("pitch");

static int s_viewshedCount = 0;

constexpr double c_defaultOffsetZ = 5.0;
constexpr double c_defaultIdentifyTolerance = 5.0;
constexpr quint8 c_defaultIdentifyMaxRecords = 5;

/*!
  \class Dsa::ViewshedController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for creating viewshed analysis.

  This tool allows viewshed analysis to be created:

  \list
    \li From the app's current position.
    \li From a supplied tap location.
    \li From a supplied GeoElement.
  \endlist

  In addition, viewsheds can be either normal (up to 120 degrees of arc)
  or 360 degree mode.
 */

/*!
  \brief Constructor accepting an optional \a parent.
 */
ViewshedController::ViewshedController(QObject* parent) :
  AbstractTool(parent),
  m_analysisOverlay(new AnalysisOverlay(this)),
  m_viewsheds(new ViewshedListModel(this))
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged, this, [this]
  {
    setSceneView(dynamic_cast<SceneView*>(ToolResourceProvider::instance()->geoView()));
  });

  connectMouseSignals();

  connect(m_viewsheds, &ViewshedListModel::viewshedRemoved, this, [this](Viewshed360* viewshed)
  {
    std::unique_ptr<Viewshed360> viewshedPtr(viewshed);

    // remove viewshed from analysis overlay and delete object
    viewshedPtr->removeFromOverlay();

    if (viewshed == m_locationDisplayViewshed)
    {
      m_locationDisplayViewshed = nullptr;
      emit locationDisplayViewshedActiveChanged();
    }
  });

  // this tool must be in the tool manager before adding analyses below
  ToolManager::instance().addTool(this);

  auto sceneView = dynamic_cast<SceneView*>(ToolResourceProvider::instance()->geoView());
  if (sceneView)
  {
    m_sceneView = sceneView;
    m_sceneView->analysisOverlays()->append(m_analysisOverlay);
  }
}

/*!
  \brief Destructor.
 */
ViewshedController::~ViewshedController()
{
}

/*!
  \brief Sets the \l Esri::ArcGISRuntime::SceneView to \a sceneView.
 */
void ViewshedController::setSceneView(SceneView* sceneView)
{
  if (!sceneView)
    return;

  m_sceneView = sceneView;

  if (!m_sceneView->analysisOverlays()->contains(m_analysisOverlay))
    m_sceneView->analysisOverlays()->append(m_analysisOverlay);
}

/*!
  \internal
 */
void ViewshedController::connectMouseSignals()
{
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::mouseClicked, this, &ViewshedController::onMouseClicked);
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::mouseMoved, this, &ViewshedController::onMouseMoved);
}

/*!
  \brief Handler for the mouse click \a event.

  Depending on the active mode (e.g. type of viewshed that the tool is creating)
  the event will be handled differently.

  In \c AddLocationViewshed360 mode, a new location viewshed will be created at the clicked position.

  In \c AddGeoElementViewshed360 mode, an identify operation will be started to find a suitable graphic
  for the viewshed.
 */
void ViewshedController::onMouseClicked(QMouseEvent& event)
{
  if (!isActive() || !m_sceneView)
    return;

  const auto position = event.position();

  switch (m_activeMode)
  {
    case AddLocationViewshed360:
    {
      const Point pt = m_sceneView->screenToBaseSurface(position.x(), position.y());
      addLocationViewshed360(pt);
      return;
    }
    case AddGeoElementViewshed360:
    {
      // invoke the identify operations on the sceneview for layers and graphics overlays
      auto layersIdentify = m_sceneView->identifyLayersAsync(position, c_defaultIdentifyTolerance, false, c_defaultIdentifyMaxRecords, this);
      auto graphicsOverlayIdentify = m_sceneView->identifyGraphicsOverlaysAsync(position, c_defaultIdentifyTolerance, false, c_defaultIdentifyMaxRecords, this);

      // respond once all QFutures are complete (includes any cancel or failure as well)
      QtFuture::whenAll(layersIdentify, graphicsOverlayIdentify).then(this, [this](const QList<IdentifyResultsVariant::FutureType>& identifyResults)
      {
        // bail out if the tool became deactivated
        if (!isActive() || !m_sceneView)
          return;

        for (const IdentifyResultsVariant::FutureType& identifyResult : identifyResults)
        {
          if (identifyResult.index() == IdentifyResultsVariant::Types::LAYERS)
          {
            LayerResultsManager resultsManager{std::get<IdentifyResultsVariant::Types::LAYERS>(identifyResult).result()};
            for (auto* result : resultsManager.m_results)
            {
              // make sure the results for the current layer has at least one element in it
              auto geoElements = result->geoElements();
              if (!result || geoElements.count() < 1)
                continue;

              // loop through every element in the result
              for (auto* geoElement : geoElements)
              {
                // skip anything that is not a point
                if (geoElement->geometry().geometryType() != GeometryType::Point)
                  continue;

                // if the type returned was a dynamic entity observation then get the dynamic entity to use as the geoelement
                if (auto* dynamicEntityObservation = dynamic_cast<DynamicEntityObservation*>(geoElement); dynamicEntityObservation)
                {
                  addGeoElementViewshed360(dynamicEntityObservation->dynamicEntity());
                  return;
                }

                // any other types can be added via the geoelement itself
                addGeoElementViewshed360(geoElement);
                return;
              }
            }
          }
          else if (identifyResult.index() == IdentifyResultsVariant::Types::GRAPHICS)
          {
            GraphicsOverlaysResultsManager resultsManager{std::get<IdentifyResultsVariant::Types::GRAPHICS>(identifyResult).result()};
            for (auto* result : resultsManager.m_results)
            {
              // make sure the results for the current layer has at least one element in it
              auto geoElements = result->geoElements();
              if (!result || geoElements.count() < 1)
                continue;

              // loop through every element in the result
              for (auto* geoElement : geoElements)
              {
                // skip anything that is not a point
                if (geoElement->geometry().geometryType() != GeometryType::Point)
                  continue;

                addGeoElementViewshed360(geoElement);
                return;
              }
            }
          }
        }
      });
      break;
    }
    default:
      break;
  }
}

/*!
  \brief React to the mouse moved \a event.

  This event will only be handled if the active mode is \c AddLocationViewshed360 mode.
  In this mode, when there is already an existing location viewshed, it's position will
  be updated to follow the current mouse position.
 */
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

  const Point point = m_sceneView->screenToBaseSurface(event.position().x(), event.position().y());
  locViewshed->setPoint(point);

  event.accept();
}

/*!
  \brief Add a new viewshed centered upon the app's current position.
 */
void ViewshedController::addLocationDisplayViewshed()
{
  if (m_locationDisplayViewshed)
    return;

  LocationController* locationController = ToolManager::instance().tool<LocationController>();
  if (!locationController)
    return;

  Graphic* locationGraphic = locationController->locationDisplay()->locationGraphic();
  m_locationDisplayViewshed = new GeoElementViewshed360(locationGraphic, m_analysisOverlay, VIEWSHED_HEADING_ATTRIBUTE, VIEWSHED_PITCH_ATTRIBUTE, this);
  m_locationDisplayViewshed->setName(QStringLiteral("Location Display Viewshed"));
  m_locationDisplayViewshed->setOffsetZ(c_defaultOffsetZ);
  m_analysisOverlay->analyses()->append(m_locationDisplayViewshed->viewshed());
  m_viewsheds->append(m_locationDisplayViewshed);

  m_activeViewshed = m_locationDisplayViewshed;
  emit locationDisplayViewshedActiveChanged();
  updateActiveViewshed();
}

/*!
  \brief Add a new viewshed centered upon the supplied \a point.
 */
void ViewshedController::addLocationViewshed360(const Esri::ArcGISRuntime::Point& point)
{
  if (!m_graphicsOverlay)
  {
    if (!m_sceneView)
      return;

    // create a graphics overlay and use a renderer to display a red cone symbol indicating
    // the orientation of the viewshed (not used in 360 degree mode).
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

  // clear any existing camera contollers.
  if (m_followCamCtrllr)
  {
    delete m_followCamCtrllr;
    m_followCamCtrllr = nullptr;
  }

  if (m_sceneView)
  {
    // set the scene to use a OrbitLocationCameraController so that panning in the view does not cause
    // navigation - but can instead be used for exploratory analysis (see onMouseMoved).
    const Camera currCam = m_sceneView->currentViewpointCamera();
    const double currDistance = DsaUtility::distance3D(currCam.location(), currCam.location());
    m_followCamCtrllr = new OrbitLocationCameraController(currCam.location(), currDistance, this);
    m_followCamCtrllr->setCameraPitchOffset(currCam.pitch());
    m_followCamCtrllr->setCameraHeadingOffset(currCam.heading());
    m_followCamCtrllr->setCameraHeadingOffsetInteractive(false);
    m_followCamCtrllr->setCameraPitchOffsetInteractive(false);

    m_sceneView->setCameraController(m_followCamCtrllr);
  }

  m_activeViewshed = locationViewshed360;
  updateActiveViewshed();
}

/*!
  \brief Add a new viewshed centered upon the supplied \a geoElement.
 */
void ViewshedController::addGeoElementViewshed360(GeoElement* geoElement)
{
  removeActiveViewshed();

  auto geoElementViewshed360 = new GeoElementViewshed360(geoElement, m_analysisOverlay, QString(), QString(), this);
  s_viewshedCount++;
  geoElementViewshed360->setName(QString("Viewshed %1").arg(QString::number(s_viewshedCount)));
  if (!GeoElementUtils::toQObject(geoElement)->parent())
    GeoElementUtils::setParent(geoElement, geoElementViewshed360);

  geoElementViewshed360->setOffsetZ(c_defaultOffsetZ);
  m_analysisOverlay->analyses()->append(geoElementViewshed360->viewshed());
  m_viewsheds->append(geoElementViewshed360);

  m_activeViewshed = geoElementViewshed360;
  updateActiveViewshed();
}

/*!
  \property ViewshedController::locationDisplayViewshedActive
  \brief Returns whether a viewshed exists for the app's current position.
 */
bool ViewshedController::isLocationDisplayViewshedActive() const
{
  return m_locationDisplayViewshed != nullptr;
}

/*!
  \property ViewshedController::activeMode
  \brief Returns the active mode for the tool - that is the kind of viewshed
  which will be created.
 */
ViewshedController::ViewshedActiveMode ViewshedController::activeMode() const
{
  return m_activeMode;
}

/*!
  \brief Sets the active mode for the tool to \a mode.
 */
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

/*!
  \property ViewshedController::viewsheds
  \brief Returns a model containing the current list of viewsheds.
 */
QAbstractListModel* ViewshedController::viewsheds() const
{
  return m_viewsheds;
}

/*!
  \brief Returns the name of this tool - \c "viewshed".
 */
QString ViewshedController::toolName() const
{
  return QStringLiteral("viewshed");
}

/*!
  \brief Returns the active viewshed.
 */
Viewshed360* ViewshedController::activeViewshed() const
{
  return m_activeViewshed;
}

/*!
  \brief Removes the active viewshed.
 */
void ViewshedController::removeActiveViewshed()
{
  if (!m_activeViewshed)
    return;

  m_viewsheds->removeOne(m_activeViewshed);
  m_activeViewshed = nullptr;

  updateActiveViewshed();
}

/*!
  \brief Finishes the active viewshed.

  Once a viewshed has been "finished" it can no longer be edited by this tool.
 */
void ViewshedController::finishActiveViewshed()
{
  m_activeViewshed = nullptr;
}

/*!
  \property ViewshedController::activeViewshedEnabled
  \brief Returns whether there is an active viewshed.
 */
bool ViewshedController::isActiveViewshedEnabled() const
{
  return m_activeViewshed != nullptr;
}

/*!
  \property ViewshedController::activeViewshedMinDistance
  \brief Returns the minimum distance of the active viewshed in meters.

  If there is no active viewshed this will be \c NAN.
 */
double ViewshedController::activeViewshedMinDistance() const
{
  return m_activeViewshed ? m_activeViewshed->minDistance() : NAN;
}

/*!
  \brief Sets the minimum distance of the active viewshed to \a minDistance meters.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshedMinDistance(double minDistance)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->setMinDistance(minDistance);
}

/*!
  \property ViewshedController::activeViewshedMaxDistance
  \brief Returns the maximum distance of the active viewshed in meters.

  If there is no active viewshed this will be \c NAN.
 */
double ViewshedController::activeViewshedMaxDistance() const
{
  return m_activeViewshed ? m_activeViewshed->maxDistance() : NAN;
}

/*!
  \brief Sets the maximum distance of the active viewshed to \a maxDistance meters.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshedMaxDistance(double maxDistance)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->setMaxDistance(maxDistance);
}

/*!
  \property ViewshedController::activeViewshedHorizontalAngle
  \brief Returns the horizontal angle of the active viewshed in degrees.

  If there is no active viewshed this will be \c NAN.
 */
double ViewshedController::activeViewshedHorizontalAngle() const
{
  return m_activeViewshed ? m_activeViewshed->horizontalAngle() : NAN;
}

/*!
  \brief Sets the horizontal angle of the active viewshed to \a horizontalAngle degrees.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshedHorizontalAngle(double horizontalAngle)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->horizontalAngle() == horizontalAngle)
    return;

  m_activeViewshed->setHorizontalAngle(horizontalAngle);
}

/*!
  \property ViewshedController::activeViewshedVerticalAngle
  \brief Returns the vertical angle of the active viewshed in degrees.

  If there is no active viewshed this will be \c NAN.
 */
double ViewshedController::activeViewshedVerticalAngle() const
{
  return m_activeViewshed ? m_activeViewshed->verticalAngle() : NAN;
}

/*!
  \brief Sets the vertical angle of the active viewshed to \a verticalAngle degrees.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshedVerticalAngle(double verticalAngle)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->verticalAngle() == verticalAngle)
    return;

  m_activeViewshed->setVerticalAngle(verticalAngle);
}

/*!
  \property ViewshedController::activeViewshedHeading
  \brief Returns the heading of the active viewshed in degrees.

  If there is no active viewshed this will be \c NAN.
 */
double ViewshedController::activeViewshedHeading() const
{
  return m_activeViewshed ? m_activeViewshed->heading() : NAN;
}

/*!
  \brief Sets the heading of the active viewshed to \a heading degrees.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshedHeading(double heading)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->heading() == heading)
    return;

  m_activeViewshed->setHeading(heading);
}

/*!
  \property ViewshedController::activeViewshedPitch
  \brief Returns the pitch of the active viewshed in degrees.

  If there is no active viewshed this will be \c NAN.
 */
double ViewshedController::activeViewshedPitch() const
{
  return m_activeViewshed ? m_activeViewshed->pitch() : NAN;
}

/*!
  \brief Sets the pitch of the active viewshed to \a pitch degrees.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshedPitch(double pitch)
{
  if (!m_activeViewshed)
    return;

  if (m_activeViewshed->pitch() == pitch)
    return;

  m_activeViewshed->setPitch(pitch);
}

/*!
  \property ViewshedController::activeViewshedMinPitch
  \brief Returns the minimum pitch of the active viewshed in degrees.

  If there is no active viewshed this will be \c NAN.
 */
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

/*!
  \property ViewshedController::activeViewshedMaxPitch
  \brief Returns the maximum pitch of the active viewshed in degrees.

  If there is no active viewshed this will be \c NAN.
 */
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

/*!
  \property ViewshedController::activeViewshedOffsetZ
  \brief Returns the offset z of the active viewshed in meters.

  If there is no active viewshed this will be \c 0.0.
 */
double ViewshedController::activeViewshedOffsetZ() const
{
  constexpr double offsetZDefault = 0.0;
  return m_activeViewshed ? m_activeViewshed->offsetZ() : offsetZDefault;
}

/*!
  \brief Sets the offset z of the active viewshed to \a offsetZ meters.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshedOffsetZ(double offsetZ)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->setOffsetZ(offsetZ);
}

/*!
  \property ViewshedController::activeViewshedHeadingEnabled
  \brief Returns whether heading is enabled for the active viewshed.

  If there is no active viewshed this will be \c false.
 */
bool ViewshedController::isActiveViewshedHeadingEnabled() const
{
  return m_activeViewshed ? m_activeViewshed->isHeadingEnabled() : false;
}

/*!
  \brief Returns whether pitch is enabled for the active viewshed.

  If there is no active viewshed this will be \c false.
 */
bool ViewshedController::isActiveViewshedPitchEnabled() const
{
  return m_activeViewshed ? m_activeViewshed->isPitchEnabled() : false;
}

/*!
  \property ViewshedController::activeViewshedOffsetZEnabled
  \brief Returns whether offset z is enabled for the active viewshed.

  If there is no active viewshed this will be \c false.
 */
bool ViewshedController::isActiveViewshedOffsetZEnabled() const
{
  return m_activeViewshed ? m_activeViewshed->isOffsetZEnabled() : false;
}

/*!
  \property ViewshedController::isActiveViewshed360Mode
  \brief Returns whether the active viewshed is in 360 degree mode.

  If there is no active viewshed this will be \c true.
 */
bool ViewshedController::isActiveViewshed360Mode() const
{
  return m_activeViewshed ? m_activeViewshed->is360Mode() : true;
}

/*!
  \brief Sets the whether the active viewshed is in 360 degree mode to \a is360Mode.

  If there is no active viewshed this will be ignored.
 */
void ViewshedController::setActiveViewshed360Mode(bool is360Mode)
{
  if (!m_activeViewshed)
    return;

  m_activeViewshed->set360Mode(is360Mode);
}

/*!
  \internal
 */
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

/*!
  \internal
 */
void ViewshedController::updateActiveViewshed()
{
  if (!m_activeViewshed)
  {
    disconnectActiveViewshedSignals();
    emitActiveViewshedSignals();
    return;
  }

  updateActiveViewshedSignals();
  emitActiveViewshedSignals();
}

/*!
  \internal
 */
void ViewshedController::disconnectActiveViewshedSignals()
{
  if (!m_activeViewshedConns.isEmpty())
  {
    for (const auto& conn : qAsConst(m_activeViewshedConns))
    {
      disconnect(conn);
    }

    m_activeViewshedConns.clear();
  }
}

/*!
  \internal
 */
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

} // Dsa

// Signal Documentation
/*!
  \fn void ViewshedController::locationDisplayViewshedActiveChanged();
  \brief Signal emitted when the LocationDisplay viewshed active property changes.
 */

/*!
  \fn void ViewshedController::activeViewshed360ModeChanged();
  \brief Signal emitted when the currently active viewshed is in 360 mode.
 */

/*!
  \fn void ViewshedController::activeViewshedOffsetEnabledChanged();
  \brief Signal emitted when the currently active viewshed changes whether offsets are enabled.
 */

/*!
  \fn void ViewshedController::activeViewshedPitchEnabledChanged();
  \brief Signal emitted when the currently active viewshed changes whether pitch is enabled.
 */

/*!
  \fn void ViewshedController::activeViewshedHeadingEnabledChanged();
  \brief Signal emitted when the currently active viewshed changes whether heading is enabled.
 */

/*!
  \fn void ViewshedController::activeViewshedOffsetZChanged();
  \brief Signal emitted when the currently active viewshed changes offset z-values.
 */

/*!
  \fn void ViewshedController::activeViewshedMaxPitchChanged();
  \brief Signal emitted when the currently active viewshed changes maximum pitch.
 */

/*!
  \fn void ViewshedController::activeViewshedMinPitchChanged();
  \brief Signal emitted when the currently active viewshed changes minimum pitch.
 */

/*!
  \fn void ViewshedController::activeViewshedPitchChanged();
  \brief Signal emitted when the currently active viewshed changes pitch.
 */

/*!
  \fn void ViewshedController::activeViewshedHeadingChanged();
  \brief Signal emitted when the currently active viewshed changes heading.
 */

/*!
  \fn void ViewshedController::activeViewshedVerticalAngleChanged();
  \brief Signal emitted when the currently active viewshed changes vertical angle.
 */

/*!
  \fn void ViewshedController::activeViewshedHorizontalAngleChanged();
  \brief Signal emitted when the currently active viewshed changes horizontal angle.
 */

/*!
  \fn void ViewshedController::activeViewshedMaxDistanceChanged();
  \brief Signal emitted when the currently active viewshed changes maximum distance.
 */

/*!
  \fn void ViewshedController::activeViewshedMinDistanceChanged();
  \brief Signal emitted when the currently active viewshed changes minimum distance.
 */

/*!
  \fn void ViewshedController::activeViewshedEnabledChanged();
  \brief Signal emitted when currently active viewshed enabled changes.
 */

/*!
  \fn void ViewshedController::activeModeChanged();
  \brief Signal emitted when the active mode changes.
 */
