// Copyright 2018 ESRI
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


// PCH header
#include "pch.hpp"

#include "ContextMenuController.h"

// example app headers
#include "ObservationReportController.h"
#include "FollowPositionController.h"
#include "GraphicsOverlaysResultsManager.h"
#include "IdentifyController.h"
#include "LayerResultsManager.h"
#include "LineOfSightController.h"
#include "ViewshedController.h"

// toolkit headers
#include "CoordinateConversionController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "MapView.h"
#include "SceneView.h"

// STL headers
#include <cmath>

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
  \class ContextMenuController
  \inherits Toolkit::AbstractTool
  \brief Tool controller for displaying a Context menu.

  When the user presses and holds the mouse, a number of tasks are started
  to discover the current context for the app. Based on the result of these
  operations a set of context specific operations are presented. For example,
  the tool offers options such as:

  \list
    \li Identify.
    \li Elevation.
    \li Coordinates.
    \li Contact Report.
    \li Viewshed.
    \li Line of sight.
  \endlist

  \sa ContactReportController
  \sa IdentifyController
  \sa ViewshedController
  \sa LineOfSightController
  \sa Esri::ArcGISRuntime::Toolkit::CoordinateConversionController
 */

/*!
  \brief Constructor accepting an optional \a parent.
 */
ContextMenuController::ContextMenuController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_options(new QStringListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  Toolkit::ToolResourceProvider* resourceProvider = Toolkit::ToolResourceProvider::instance();
  // setup connection to handle mouse-clicking in the view (used to trigger the identify tasks)
  connect(resourceProvider, &Toolkit::ToolResourceProvider::mousePressedAndHeld,
          this, &ContextMenuController::onMousePressedAndHeld);

  // setup connection to handle the results of an Identify Layers task
  connect(resourceProvider, &Toolkit::ToolResourceProvider::identifyLayersCompleted,
          this, &ContextMenuController::onIdentifyLayersCompleted);

  // setup connection to handle the results of an Identify Graphic Overlays task
  connect(resourceProvider, &Toolkit::ToolResourceProvider::identifyGraphicsOverlaysCompleted,
          this, &ContextMenuController::onIdentifyGraphicsOverlaysCompleted);

  // setup connection to handle the results of a screen to location task
  connect(resourceProvider, &Toolkit::ToolResourceProvider::screenToLocationCompleted,
          this, &ContextMenuController::onScreenToLocationCompleted);

  m_active = true;
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

  // only consider left clicks (or taps) on the view.
  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  cancelTasks();
  clearOptions();
  for(const auto& feats : qAsConst(m_contextFeatures))
    qDeleteAll(feats);
  m_contextFeatures.clear();

  for(const auto& graphics : qAsConst(m_contextGraphics))
    qDeleteAll(graphics);
  m_contextGraphics.clear();

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  setContextScreenPosition(event.pos());

  // start tasks to determine the clicked location
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (sceneView)
  {
    m_screenToLocationTask = sceneView->screenToLocation(m_contextScreenPosition.x(), m_contextScreenPosition.y());
    m_contextBaseSurfaceLocation = sceneView->screenToBaseSurface(m_contextScreenPosition.x(), m_contextScreenPosition.y());
  }
  else
  {
    MapView* mapView = dynamic_cast<MapView*>(Toolkit::ToolResourceProvider::instance()->geoView());
    if (mapView)
    {
      const Point p = mapView->screenToLocation(m_contextScreenPosition.x(), m_contextScreenPosition.y());
      setContextLocation(p);
      m_contextBaseSurfaceLocation = p;
    }
  }

  // start tasks to determine whether a GeoElement was clicked on
  m_identifyGraphicsTask = geoView->identifyGraphicsOverlays(m_contextScreenPosition.x(), m_contextScreenPosition.y(), 5.0, false, 1);
  m_identifyFeaturesTask = geoView->identifyLayers(m_contextScreenPosition.x(), m_contextScreenPosition.y(), 5.0, false, 1);

  // accept the event to prevent it being used by other tools etc.
  event.accept();
}

/*!
  \internal

  Handle the result of an identify layers task.
 */
void ContextMenuController::onIdentifyLayersCompleted(const QUuid& taskId, const QList<IdentifyLayerResult*>& identifyResults)
{
  if (taskId != m_identifyFeaturesTask.taskId())
    return;

  LayerResultsManager resultsManager(identifyResults);

  m_identifyFeaturesTask = TaskWatcher();

  // iterate over the results and if we find a GeoElement use it for the current context
  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyLayerResult* res = *it;
    if (!res)
      continue;

    auto geoElements = res->geoElements();
    for(GeoElement* geoElement : qAsConst(geoElements))
      geoElement->setParent(this); // set the GeoElements to be managed by the tool

    // add the geoElements to the context hash using the layer name as the key
    m_contextFeatures.insert(res->layerContent()->name(), geoElements);
  }

  processGeoElements();
}

/*!
  \internal

  Handle the result of an identify graphics overlays task.
 */
void ContextMenuController::onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, const QList<IdentifyGraphicsOverlayResult*>& identifyResults)
{
  if (taskId != m_identifyGraphicsTask.taskId())
    return;

  GraphicsOverlaysResultsManager resultsManager(identifyResults);

  m_identifyGraphicsTask = TaskWatcher();

  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyGraphicsOverlayResult* res = *it;
    if (!res)
      continue;

    const QList<Graphic*> graphics = res->graphics();
    if (graphics.isEmpty())
      continue;

    QList<GeoElement*> geoElements;
    auto gIt = graphics.begin();
    auto gEnd = graphics.end();
    for(; gIt != gEnd; ++gIt)
    {
      GeoElement* geoElement = *gIt;
      geoElement->setParent(this); // set the GeoElements to be managed by the tool
      geoElements.append(geoElement);
    }

    // add the geoElements to the context hash using the overlay id as the key
    m_contextGraphics.insert(res->graphicsOverlay()->overlayId(), geoElements);
  }

  processGeoElements();
}


/*!
  \internal

  Handle the result of a screen to location task.
 */
void ContextMenuController::onScreenToLocationCompleted(QUuid taskId, const Point& location)
{
  if (taskId != m_screenToLocationTask.taskId())
    return;

  m_screenToLocationTask = TaskWatcher();
  setContextLocation(location);
}


/*!
  \brief Update the context information for the clicked screen position.
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

  Cancels any currently running tasks.
 */
void ContextMenuController::cancelTasks()
{
  cancelIdentifyTasks();
  m_screenToLocationTask.cancel();
  m_screenToLocationTask = TaskWatcher();
}

/*!
  \internal

  Cancels any currently running identify tasks.
 */
void ContextMenuController::cancelIdentifyTasks()
{
  m_identifyFeaturesTask.cancel();
  m_identifyFeaturesTask = TaskWatcher();
  m_identifyGraphicsTask.cancel();
  m_identifyGraphicsTask = TaskWatcher();
}

/*!
  \internal
 */
void ContextMenuController::processGeoElements()
{
  // if either of the identify tasks is still in progress, return.
  if ((m_identifyFeaturesTask.isValid() && !m_identifyFeaturesTask.isDone()) ||
      (m_identifyGraphicsTask.isValid() && !m_identifyGraphicsTask.isDone()))
    return;

  if (m_contextFeatures.isEmpty() && m_contextGraphics.isEmpty())
    return;

  // if we have at least 1 GeoElement, we can identify
  addOption(IDENTIFY_OPTION);

  int pointGraphicsCount = 0;
  for (const auto& geoElements : qAsConst(m_contextGraphics))
  {
    for (GeoElement* geoElement : geoElements)
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
  for (const auto& geoElements : qAsConst(m_contextFeatures))
  {
    for (GeoElement* geoElement : geoElements)
    {
      if (geoElement && geoElement->geometry().geometryType() == GeometryType::Point)
      {
        addOption(LINE_OF_SIGHT_OPTION);
        return;
      }
    }
  }
}

/*!
  \brief Returns the geographic location for the current context.
 */
Point ContextMenuController::contextLocation() const
{
    return m_contextLocation;
}

/*!
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
    IdentifyController* identifyTool = Toolkit::ToolManager::instance().tool<IdentifyController>();
    if (!identifyTool)
      return;

    auto combinedGeoElementsByTitle = m_contextGraphics;
    combinedGeoElementsByTitle.unite(m_contextFeatures);
    identifyTool->showPopups(combinedGeoElementsByTitle);
  }
  else if (option == VIEWSHED_OPTION)
  {
    ViewshedController* viewshedTool = Toolkit::ToolManager::instance().tool<ViewshedController>();
    if (!viewshedTool)
      return;

    viewshedTool->setActiveMode(ViewshedController::ViewshedActiveMode::AddLocationViewshed360);
    viewshedTool->addLocationViewshed360(m_contextBaseSurfaceLocation);
    viewshedTool->finishActiveViewshed();
    viewshedTool->setActiveMode(ViewshedController::ViewshedActiveMode::NoActiveMode);
  }
  else if (option == FOLLOW_OPTION)
  {
    FollowPositionController* followTool = Toolkit::ToolManager::instance().tool<FollowPositionController>();
    if (!followTool)
      return;

    // follow the 1st point graphic (should be only 1)
    for(const auto& geoElements : qAsConst(m_contextGraphics))
    {
      for (GeoElement* geoElement : geoElements)
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
    Toolkit::CoordinateConversionController* coordinateTool = Toolkit::ToolManager::instance().tool<Toolkit::CoordinateConversionController>();
    if (!coordinateTool)
      return;

    coordinateTool->setRunConversion(true);
    coordinateTool->setCaptureMode(true);
    coordinateTool->setPointToConvert(m_contextLocation);
    coordinateTool->setActive(true);
  }
  else if (option == LINE_OF_SIGHT_OPTION)
  {
    LineOfSightController* lineOfSightTool = Toolkit::ToolManager::instance().tool<LineOfSightController>();
    if (!lineOfSightTool)
      return;

    // perform LOS to each point geoElement found
    // follow the 1st point graphic (should be only 1)
    auto losFunc = [lineOfSightTool](const QHash<QString, QList<GeoElement*>>& geoElementsByTitle)
    {
      for(auto gIt = geoElementsByTitle.cbegin(); gIt != geoElementsByTitle.cend(); ++gIt)
      {
        const QList<GeoElement*>& geoElements = gIt.value();
        for (GeoElement* geoElement : qAsConst(geoElements))
        {
          if (!geoElement || geoElement->geometry().geometryType() != GeometryType::Point)
            continue;

          lineOfSightTool->lineOfSightFromLocationToGeoElement(geoElement);
        }
      }
    };

    losFunc(m_contextGraphics);
    losFunc(m_contextFeatures);
  }
  else if (option == OBSERVATION_REPORT_OPTION)
  {
    Dsa::ObservationReportController* observationReportTool = Toolkit::ToolManager::instance().tool<Dsa::ObservationReportController>();
    if (!observationReportTool)
      return;

    observationReportTool->setControlPoint(m_contextLocation);
    observationReportTool->setActive(true);
  }
}

/*!
  \brief Returns the screen position for the current context.
 */
QPoint ContextMenuController::contextScreenPosition() const
{
  return m_contextScreenPosition;
}

/*!
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

