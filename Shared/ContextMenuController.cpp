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

#include "ContextMenuController.h"

// C++ API
#include "DynamicEntity.h"
#include "DynamicEntityIterator.h"
#include "DynamicEntityQueryParameters.h"
#include "DynamicEntityQueryResult.h"
#include "DynamicEntityObservation.h"
#include "GlobeCameraController.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "IdentifyLayerResult.h"
#include "LayerContent.h"
#include "MapView.h"
#include "SceneView.h"
#include "Viewpoint.h"
// Toolkit
#include "CoordinateConversionController.h"
// DSA
#include "AppConstants.h"
#include "CoordinateConversionToolProxy.h"
#include "FollowPositionController.h"
#include "GeoElementUtils.h"
#include "GraphicsOverlaysResultsManager.h"
#include "IdentifyController.h"
#include "LayerResultsManager.h"
#include "LineOfSightController.h"
#include "MessageFeedsController.h"
#include "ObservationReportController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "ViewshedController.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::ContextMenuController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for displaying a Context menu.

  When the user presses and holds the mouse, a number of tasks are started
  to discover the current context for the app. Based on the result of these
  operations a set of context specific operations are presented. For example,
  the tool offers options such as:

  \list
    \li Identify.
    \li Elevation.
    \li Coordinates.
    \li Observation Report.
    \li Viewshed.
    \li Line of sight.
  \endlist

  \sa ObservationReportController
  \sa IdentifyController
  \sa ViewshedController
  \sa LineOfSightController
  \sa Esri::ArcGISRuntime::CoordinateConversionController
 */

/*!
  \brief Constructor accepting an optional \a parent.
 */
ContextMenuController::ContextMenuController(QObject* parent /* = nullptr */):
  AbstractTool(parent),
  m_options(new QStringListModel(this))
{
  ToolResourceProvider* trp = ToolResourceProvider::instance();
  connect(trp, &ToolResourceProvider::geoViewChanged, this, [this]()
  {
    if (m_messageFeedsControllerConnection)
      return;

    MessageFeedsController* mfc = ToolManager::instance().tool<MessageFeedsController>();
    m_messageFeedsControllerConnection = connect(mfc,
                                                 &MessageFeedsController::entitySelected,
                                                 this,
                                                 [this](const QString& entityId, MessageFeed* messageFeed)
    {
      clearOptions();

      GeoView* geoView = ToolResourceProvider::instance()->geoView();
      if (!geoView)
        return;

      // stop if following something already by replacing the camera controller
      SceneView* sceneView = dynamic_cast<SceneView*>(geoView);
      if (!sceneView)
        return;
      if (CameraController* cameraController = sceneView->cameraController(); cameraController)
      {
        sceneView->setCameraController(new GlobeCameraController(this));
        delete cameraController;
      }

      DynamicEntityQueryParameters* params = new DynamicEntityQueryParameters(this);
      params->setTrackIds(QStringList{entityId});
      messageFeed->queryDynamicEntitiesAsync(params, this).then(this, [this, geoView, params, messageFeed](DynamicEntityQueryResult* result)
      {
        params->deleteLater();
        const QList<DynamicEntity*> entities = result->iterator().asList();
        if (!entities.empty())
        {
          QList<GeoElement*> geoElements{};
          std::for_each(std::cbegin(entities), std::cend(entities), [&](DynamicEntity* entity)
          {
            geoElements.push_back(entity);
          });
          geoView->setViewpointAsync(Viewpoint{geoElements.first()->geometry()}, 0.1f);
          setContextScreenPosition(QPoint{geoView->widthInPixels() / 2, geoView->heightInPixels() / 2});
          m_contextGeoElements.insert(messageFeed->feedName(), geoElements);
          processGeoElements();
        }

        result->deleteLater();
      });
    });
  });

  // setup connection to handle mouse-clicking in the view (used to trigger the identify tasks)
  connect(trp, &ToolResourceProvider::mousePressedAndHeld, this, &ContextMenuController::onMousePressedAndHeld);

  m_active = true;
  ToolManager::instance().addTool(this);
}

/*!
  \brief Destructor.
 */
ContextMenuController::~ContextMenuController()
{
}

/*!
  \brief The name of this tool.
 */
QString ContextMenuController::toolName() const
{
  return QStringLiteral("context menu");
}

/*!
  \brief Handles a mouse-click event in the view - used to set the current context.

  The context will be based upon:

  \list
    \li whether the clicked position is a valid Geographic location
    \li whether a \l Esri::ArcGISRuntime::GeoElement was clicked on
    \li whether a \l Esri::ArcGISRuntime::Graphic was clicked on
  \endlist
 */
void ContextMenuController::onMousePressedAndHeld(QMouseEvent& event)
{
  if (!isActive())
    return;

  clearOptions();

  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  setContextScreenPosition(event.pos());

  // start tasks to determine the clicked location
  if (auto* sceneView = dynamic_cast<SceneView*>(ToolResourceProvider::instance()->geoView()); sceneView)
  {
    sceneView->screenToLocationAsync(m_contextScreenPosition.x(), m_contextScreenPosition.y()).then(this, [this, sceneView](Point point) {
      setContextLocation(point);
      m_contextBaseSurfaceLocation = sceneView->screenToBaseSurface(m_contextScreenPosition.x(), m_contextScreenPosition.y());
      invokeIdentifyOnGeoView();
    });
  }
  else if (auto* mapView = dynamic_cast<MapView*>(ToolResourceProvider::instance()->geoView()); mapView)
  {
    const Point point = mapView->screenToLocation(m_contextScreenPosition.x(), m_contextScreenPosition.y());
    setContextLocation(point);
    m_contextBaseSurfaceLocation = point;
    invokeIdentifyOnGeoView();
  }

  // accept the event to prevent it being used by other tools etc.
  event.accept();
}

/*!
  \brief Update the context information for the clicked screen position.

  \list
  \li \a contextScreenPosition - The clicked screen position.
  \endlist
 */
void ContextMenuController::setContextScreenPosition(const QPoint& contextScreenPosition)
{
  if (m_contextScreenPosition == contextScreenPosition)
    return;

  m_contextScreenPosition = contextScreenPosition;
  emit contextScreenPositionChanged();
}

/*!
  \brief Update the context information for the clicked geographic location.
 */
void ContextMenuController::setContextLocation(const Point& location)
{
  if (!location.isValid() || location.isEmpty())
    return;

  if (location.x() == 0.0 && location.y() == 0.0 && location.z() == 0.0)
    return;

  m_contextLocation = location;

  addOption(OPTION_COORDINATES);

  if (std::isnan(m_contextLocation.z()))
    return;

  addOption(OPTION_ELEVATION);
  addOption(OPTION_VIEWSHED);
  addOption(OPTION_OBSERVATION_REPORT);
}

/*!
  \internal

  Add \a option to the list of actions which are valid in this context.
 */
void ContextMenuController::addOption(const QString& option)
{
  QStringList options = m_options->stringList();
  if (options.contains(option))
    return;

  options.append(option);
  m_options->setStringList(options);
  m_options->sort(0);

  setContextActive(true);
}

/*!
  \internal

  Clear the list of actions which are valid in this context.
 */
void ContextMenuController::clearOptions()
{
  m_options->setStringList(QStringList{});

  for (const QList<GeoElement*>& geoElements : std::as_const(m_contextGeoElements))
  {
    if (geoElements.empty())
      continue;

    for (GeoElement* ge : geoElements)
    {
      if (!ge)
        continue;

      // anything that was previously identified from the view
      // that is owned by the controller should be deleted
      if (QObject* o = GeoElementUtils::toQObject(ge); o)
      {
        if (o->parent() == this)
          o->deleteLater();
      }
    }
  }
  m_contextGeoElements.clear();
}

/*!
  \property ContextMenuController::resultTitle
  \brief Returns the title of the current context action.
 */
QString ContextMenuController::resultTitle() const
{
  return m_resultTitle;
}

/*!
  \internal

  Sets the title of the current context action to \a resultTitle.
 */
void ContextMenuController::setResultTitle(const QString& resultTitle)
{
  if (m_resultTitle == resultTitle)
    return;

  m_resultTitle = resultTitle;
  emit resultTitleChanged();
}

/*!
  \internal
 */
void ContextMenuController::processGeoElements()
{
  if (m_contextGeoElements.isEmpty())
    return;

  // if we have at least 1 GeoElement, we can identify
  addOption(OPTION_IDENTIFY);

  quint8 pointGeoElementCount = 0;
  const GeoElement* lastPointGeoElementFound = nullptr;
  for (const QList<GeoElement*>& geoElements : std::as_const(m_contextGeoElements))
  {
    for (const GeoElement* geoElement : geoElements)
    {
      if (geoElement->geometry().geometryType() == GeometryType::Point)
      {
        lastPointGeoElementFound = geoElement;
        if (++pointGeoElementCount > 1)
          break;
      }
    }

    // no need to continue searching the results if more than
    // one GeoElement of type point has already been found
    if (pointGeoElementCount > 1)
      break;
  }

  if (pointGeoElementCount == 1) // if we have exactly 1 point GeoElement and it is a DynamicEntity, we can follow it
  {
    if (const auto* de = dynamic_cast<const DynamicEntity*>(lastPointGeoElementFound); de)
      addOption(OPTION_FOLLOW);
  }

  if (pointGeoElementCount > 0) // if we have at least 1 point geometry, we can perform LOS
    addOption(OPTION_LINE_OF_SIGHT);
}

void ContextMenuController::invokeIdentifyOnGeoView()
{
  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  // invoke the identify operations on the geoview for layers and graphics overlays
  auto idenfityLayers = geoView->identifyLayersAsync(m_contextScreenPosition, 5.0, false, -1, this);
  auto identifyGraphicsOverlays = geoView->identifyGraphicsOverlaysAsync(m_contextScreenPosition, 5.0, false, -1, this);

  QtFuture::whenAll(idenfityLayers, identifyGraphicsOverlays).then(this, [this](const QList<IdentifyResultsVariant::FutureType>& identifyResults)
  {
    for (const IdentifyResultsVariant::FutureType& identifyResult : identifyResults)
    {
      if (identifyResult.index() == IdentifyResultsVariant::Types::LAYERS)
      {
        const QList<Esri::ArcGISRuntime::IdentifyLayerResult*> results = std::get<IdentifyResultsVariant::Types::LAYERS>(identifyResult).result();
        for (IdentifyLayerResult* result : results)
        {
          if (!result)
            continue;

          const QList<GeoElement*> geoElementsAll = result->geoElements();
          if (geoElementsAll.isEmpty())
          {
            result->deleteLater();
            continue;
          }

          QList<GeoElement*> geoElementsToOwn{};
          QList<GeoElement*> geoElements{};
          for (GeoElement* ge : geoElementsAll)
          {
            // any non-observations (Feature, etc) should be owned
            DynamicEntityObservation* deo = dynamic_cast<DynamicEntityObservation*>(ge);
            if (!deo)
            {
              geoElementsToOwn.append(ge);
              geoElements.append(ge);
              continue;
            }

            // observations other than the latest at the time of the click
            // should also be owned
            DynamicEntity* de = deo->dynamicEntity();
            if (de->latestObservation()->observationId() != deo->observationId())
            {
              geoElementsToOwn.append(deo);
              geoElements.append(deo);
              continue;
            }

            // the remaining case is that the observation was itself the latest
            // so we add the dynamic entity instead of the observation element
            // and mark the observation as no longer needed
            geoElements.append(de);
            deo->deleteLater();
          }

          // set the observations and any other non-DynamicEntity GeoElements to be owned by the tool
          GeoElementUtils::setParent(geoElementsToOwn, this);

          // add the geoElements to the context hash using the layer name as the key
          m_contextGeoElements.insert(result->layerContent()->name(), geoElements);
          result->deleteLater();
        }
      }
      else if (identifyResult.index() == IdentifyResultsVariant::Types::GRAPHICS)
      {
        GraphicsOverlaysResultsManager resultsManager(std::get<IdentifyResultsVariant::Types::GRAPHICS>(identifyResult).result());
        for (IdentifyGraphicsOverlayResult* result : std::as_const(resultsManager.m_results))
        {
          if (!result)
            continue;

          // don't process the location on the context menu
          if (result->graphicsOverlay()->overlayId() == AppConstants::PROPERTYNAME_LAYER_NAME_SCENEVIEW_LOCATION)
            continue;

          const auto geoElements = result->geoElements();
          if (geoElements.isEmpty())
            continue;

          // set the GeoElements to be managed by the tool
          GeoElementUtils::setParent(geoElements, this);

          // add the geoElements to the context hash using the overlay id as the key
          m_contextGeoElements.insert(result->graphicsOverlay()->overlayId(), geoElements);
        }
      }
    }
    processGeoElements();
  });
}

/*!
  \brief Returns the geographic location for the current context.
 */
Point ContextMenuController::contextLocation() const
{
    return m_contextLocation;
}

/*!
  \property ContextMenuController::result
  \brief Returns the result of the current action (if applicable).
 */
QString ContextMenuController::result() const
{
  return m_result;
}

/*!
  \internal

  Sets the result of the current action to \a result.
 */
void ContextMenuController::setResult(const QString& result)
{
  m_result = result;
  emit resultChanged();
}

/*!
  \property ContextMenuController::options
  \brief Returns the current list of actions which can be performed for this context
 */
QStringListModel* ContextMenuController::options() const
{
  return m_options;
}

/*!
  \brief Selects the \a option to be run for the current context.
 */
void ContextMenuController::selectOption(const QString& option)
{
  setContextActive(false);

  if (option == OPTION_ELEVATION)
  {
    setResultTitle(QStringLiteral("Elevation"));
    setResult(QString::number(m_contextLocation.z()));
  }
  else if (option == OPTION_IDENTIFY)
  {
    IdentifyController* identifyTool = ToolManager::instance().tool<IdentifyController>();
    if (!identifyTool)
      return;

    // transfer the geoelements to the identify controller which takes ownership of them
    identifyTool->showPopups(m_contextGeoElements);

    // clear the list so that the elements will not be cleaned up by subsequent
    // long presses on the geoview
    m_contextGeoElements.clear();
  }
  else if (option == OPTION_VIEWSHED)
  {
    ViewshedController* viewshedTool = ToolManager::instance().tool<ViewshedController>();
    if (!viewshedTool)
      return;

    viewshedTool->setActiveMode(ViewshedController::ViewshedActiveMode::AddLocationViewshed360);
    viewshedTool->addLocationViewshed360(m_contextBaseSurfaceLocation);
    viewshedTool->finishActiveViewshed();
    viewshedTool->setActiveMode(ViewshedController::ViewshedActiveMode::NoActiveMode);
  }
  else if (option == OPTION_FOLLOW)
  {
    FollowPositionController* followTool = ToolManager::instance().tool<FollowPositionController>();
    if (!followTool)
      return;

    // follow the 1st point graphic (should be only 1)
    for(const auto& geoElements : std::as_const(m_contextGeoElements))
    {
      for (auto* geoElement : geoElements)
      {
        if (!geoElement || geoElement->geometry().geometryType() != GeometryType::Point)
          continue;

        // follow the 'parent' DynamicEntity if the type was a DynamicEntityObservation
        auto* geoElementToFollow = geoElement;
        if (const auto* observation = dynamic_cast<DynamicEntityObservation*>(geoElement); observation)
          geoElementToFollow = static_cast<DynamicEntity*>(observation->dynamicEntity());

        followTool->followGeoElement(geoElementToFollow);
        return;
      }
    }
  }
  else if (option == OPTION_COORDINATES)
  {
    auto coordinateTool = ToolManager::instance().tool<CoordinateConversionToolProxy>();
    if (!coordinateTool)
      return;

    coordinateTool->handleClick(m_contextLocation);
    coordinateTool->controller()->setInPickingMode(true);
    coordinateTool->setActive(true);
  }
  else if (option == OPTION_LINE_OF_SIGHT)
  {
    LineOfSightController* lineOfSightTool = ToolManager::instance().tool<LineOfSightController>();
    if (!lineOfSightTool)
      return;

    // perform LOS to each point geoElement found
    // follow the 1st point graphic (should be only 1)
    auto losFunc = [lineOfSightTool](const QHash<QString, QList<GeoElement*>>& geoElementsByTitle)
    {
      for(auto gIt = geoElementsByTitle.cbegin(); gIt != geoElementsByTitle.cend(); ++gIt)
      {
        const QList<GeoElement*>& geoElements = gIt.value();
        for (auto* geoElement : geoElements)
        {
          if (!geoElement || geoElement->geometry().geometryType() != GeometryType::Point)
            continue;

          // identify results of type observation require the dynamic entity they belong to
          if (auto* dynamicEntityObservation = dynamic_cast<DynamicEntityObservation*>(geoElement); dynamicEntityObservation)
          {
            auto* dynamicEntityGeoElement = static_cast<GeoElement*>(dynamicEntityObservation->dynamicEntity());
            lineOfSightTool->lineOfSightFromLocationToGeoElement(dynamicEntityGeoElement);
          }
          else
          {
            lineOfSightTool->lineOfSightFromLocationToGeoElement(geoElement);
          }
        }
      }
    };

    losFunc(m_contextGeoElements);
  }
  else if (option == OPTION_OBSERVATION_REPORT)
  {
    Dsa::ObservationReportController* observationReportTool = ToolManager::instance().tool<Dsa::ObservationReportController>();
    if (!observationReportTool)
      return;

    observationReportTool->setControlPoint(m_contextLocation);
    observationReportTool->setActive(true);
  }
}

/*!
  \property ContextMenuController::contextScreenPosition
  \brief Returns the screen position for the current context.
 */
QPoint ContextMenuController::contextScreenPosition() const
{
  return m_contextScreenPosition;
}

/*!
  \property ContextMenuController::contextActive
  \brief Returns whether the current context is active or not.
 */
bool ContextMenuController::isContextActive() const
{
  return m_contextActive;
}

/*!
  \brief Sets the active state of the current context to \a contextRequested.
 */
void ContextMenuController::setContextActive(bool contextRequested)
{
  if (m_contextActive == contextRequested)
    return;

  m_contextActive = contextRequested;
  emit contextActiveChanged();
}

} // Dsa

// Signal Documentation

/*!
  \fn void ContextMenuController::contextActiveChanged();

  \brief Signal emitted when active state changes.
 */

/*!
  \fn void ContextMenuController::contextScreenPositionChanged();

  \brief Signal emitted when the screen position of the context menu changes.
 */

/*!
  \fn void ContextMenuController::optionsChanged();

  \brief Signal emitted when the current options of the context menu changes.
 */

/*!
  \fn void ContextMenuController::resultChanged();

  \brief Signal emitted when the results change.
 */

/*!
  \fn void ContextMenuController::resultTitleChanged();

  \brief Signal emitted when the result title changes.
 */
