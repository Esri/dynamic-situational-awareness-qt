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

// C++ API headers
#include "DynamicEntity.h"
#include "DynamicEntityObservation.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "IdentifyLayerResult.h"
#include "LayerContent.h"
#include "MapView.h"
#include "SceneView.h"

// DSA headers
#include "AppConstants.h"
#include "CoordinateConversionController.h"
#include "CoordinateConversionToolProxy.h"
#include "FollowPositionController.h"
#include "GeoElementUtils.h"
#include "GraphicsOverlaysResultsManager.h"
#include "IdentifyController.h"
#include "LayerResultsManager.h"
#include "LineOfSightController.h"
#include "ObservationReportController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "ViewshedController.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString ContextMenuController::COORDINATES_OPTION = "Coordinates";
const QString ContextMenuController::ELEVATION_OPTION = "Elevation";
const QString ContextMenuController::FOLLOW_OPTION = "Follow";
const QString ContextMenuController::IDENTIFY_OPTION = "Identify";
const QString ContextMenuController::LINE_OF_SIGHT_OPTION = "Line of sight";
const QString ContextMenuController::VIEWSHED_OPTION = "Viewshed";
const QString ContextMenuController::OBSERVATION_REPORT_OPTION = "Observation";

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
  ToolResourceProvider* resourceProvider = ToolResourceProvider::instance();
  // setup connection to handle mouse-clicking in the view (used to trigger the identify tasks)
  connect(resourceProvider, &ToolResourceProvider::mousePressedAndHeld,
          this, &ContextMenuController::onMousePressedAndHeld);

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
  for (const auto& feats : std::as_const(m_contextFeatures))
    qDeleteAll(feats);
  m_contextFeatures.clear();

  for (const auto& graphics : std::as_const(m_contextGraphics))
    qDeleteAll(graphics);
  m_contextGraphics.clear();

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

  addOption(COORDINATES_OPTION);

  if (std::isnan(m_contextLocation.z()))
    return;

  addOption(ELEVATION_OPTION);
  addOption(VIEWSHED_OPTION);
  addOption(OBSERVATION_REPORT_OPTION);
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
  m_options->setStringList(QStringList());
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
  if (m_contextFeatures.isEmpty() && m_contextGraphics.isEmpty())
    return;

  // if we have at least 1 GeoElement, we can identify
  addOption(IDENTIFY_OPTION);

  int pointGraphicsCount = 0;
  for (const auto& geoElements : std::as_const(m_contextGraphics))
  {
    for (const auto* geoElement : geoElements)
    {
      if (geoElement->geometry().geometryType() == GeometryType::Point)
        pointGraphicsCount++;
    }
  }

  if (pointGraphicsCount == 1) // if we have exactly 1 point graphic, we can follow it
    addOption(FOLLOW_OPTION);

  if (pointGraphicsCount > 0) // if we have at least 1 point geometry, we can perform LOS
  {
    addOption(LINE_OF_SIGHT_OPTION);
    return;
  }

  // if were have 0 point graphics, check whether we have any point features
  for (const auto& geoElements : std::as_const(m_contextFeatures))
  {
    for (const auto* geoElement : geoElements)
    {
      if (geoElement && geoElement->geometry().geometryType() == GeometryType::Point)
      {
        addOption(LINE_OF_SIGHT_OPTION);
        return;
      }
    }
  }
}

void ContextMenuController::invokeIdentifyOnGeoView()
{
  GeoView* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  // invoke the identify operations on the geoview for layers and graphics overlays
  auto layers_identify = geoView->identifyLayersAsync(m_contextScreenPosition, 5.0, false, -1, this);
  auto graphics_overlay_identify = geoView->identifyGraphicsOverlaysAsync(m_contextScreenPosition, 5.0, false, -1, this);

  QtFuture::whenAll(layers_identify, graphics_overlay_identify).then(this, [this](const QList<IdentifyResultsVariant::FutureType> &identify_results)
  {
    for (const IdentifyResultsVariant::FutureType& identify_result : identify_results)
    {
      if (identify_result.index() == IdentifyResultsVariant::Types::LAYERS)
      {
        LayerResultsManager resultsManager(std::get<IdentifyResultsVariant::Types::LAYERS>(identify_result).result());
        for (auto* result : resultsManager.m_results)
        {
          if (!result)
            continue;

          auto geoElements = result->geoElements();
          // set the GeoElements to be managed by the tool
          GeoElementUtils::setParent(geoElements, this);

          // add the geoElements to the context hash using the layer name as the key
          m_contextFeatures.insert(result->layerContent()->name(), geoElements);
        }
      }
      else if (identify_result.index() == IdentifyResultsVariant::Types::GRAPHICS)
      {
        GraphicsOverlaysResultsManager resultsManager(std::get<IdentifyResultsVariant::Types::GRAPHICS>(identify_result).result());
        for (auto* result : resultsManager.m_results)
        {
          if (!result)
            continue;

          const auto graphics = result->graphics();
          if (graphics.isEmpty())
            continue;

          // don't process the location on the context menu
          if (result->graphicsOverlay()->overlayId() == AppConstants::LAYER_NAME_SCENEVIEW_LOCATION)
            continue;

          QList<GeoElement*> geoElements;
          for(auto* geoElement : graphics)
          {
            GeoElementUtils::setParent(geoElement, this); // set the GeoElements to be managed by the tool
            geoElements.append(geoElement);
          }

          // add the geoElements to the context hash using the overlay id as the key
          m_contextGraphics.insert(result->graphicsOverlay()->overlayId(), geoElements);
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

  if (option == ELEVATION_OPTION)
  {
    setResultTitle(QStringLiteral("Elevation"));
    setResult(QString::number(m_contextLocation.z()));
  }
  else if (option == IDENTIFY_OPTION)
  {
    IdentifyController* identifyTool = ToolManager::instance().tool<IdentifyController>();
    if (!identifyTool)
      return;

    auto combinedGeoElementsByTitle = m_contextGraphics;
    combinedGeoElementsByTitle.insert(m_contextFeatures);
    identifyTool->showPopups(combinedGeoElementsByTitle);
  }
  else if (option == VIEWSHED_OPTION)
  {
    ViewshedController* viewshedTool = ToolManager::instance().tool<ViewshedController>();
    if (!viewshedTool)
      return;

    viewshedTool->setActiveMode(ViewshedController::ViewshedActiveMode::AddLocationViewshed360);
    viewshedTool->addLocationViewshed360(m_contextBaseSurfaceLocation);
    viewshedTool->finishActiveViewshed();
    viewshedTool->setActiveMode(ViewshedController::ViewshedActiveMode::NoActiveMode);
  }
  else if (option == FOLLOW_OPTION)
  {
    FollowPositionController* followTool = ToolManager::instance().tool<FollowPositionController>();
    if (!followTool)
      return;

    // follow the 1st point graphic (should be only 1)
    for(const auto& geoElements : std::as_const(m_contextGraphics))
    {
      for (auto* geoElement : geoElements)
      {
        if (!geoElement || geoElement->geometry().geometryType() != GeometryType::Point)
          continue;

        followTool->followGeoElement(geoElement);
        return;
      }
    }
  }
  else if (option == COORDINATES_OPTION)
  {
    auto coordinateTool = ToolManager::instance().tool<CoordinateConversionToolProxy>();
    if (!coordinateTool)
      return;

    coordinateTool->handleClick(m_contextLocation);
    coordinateTool->controller()->setInPickingMode(true);
    coordinateTool->setActive(true);
  }
  else if (option == LINE_OF_SIGHT_OPTION)
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

    losFunc(m_contextGraphics);
    losFunc(m_contextFeatures);
  }
  else if (option == OBSERVATION_REPORT_OPTION)
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
