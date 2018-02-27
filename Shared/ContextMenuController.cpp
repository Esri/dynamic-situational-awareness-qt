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

#include "AnalysisController.h"
#include "ContextMenuController.h"
#include "FollowPositionController.h"
#include "GraphicsOverlaysResultsManager.h"
#include "IdentifyController.h"
#include "LayerResultsManager.h"

#include "CoordinateConversionController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "MapView.h"
#include "SceneView.h"

using namespace Esri::ArcGISRuntime;

const QString ContextMenuController::COORDINATES_OPTION = "Coordinates";
const QString ContextMenuController::ELEVATION_OPTION = "Elevation";
const QString ContextMenuController::FOLLOW_OPTION = "Follow";
const QString ContextMenuController::IDENTIFY_OPTION = "Identify";
const QString ContextMenuController::VIEWSHED_OPTION = "Viewshed";

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
  // only consider left clicks (or taps) on the view.
  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  cancelTasks();
  clearOptions();
  m_contextElement = nullptr;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  setContextScreenPosition(event.pos());

  // start tasks to determine the clicked location
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (sceneView)
    m_screenToLocationTask = sceneView->screenToLocation(m_contextScreenPosition.x(), m_contextScreenPosition.y());
  else
  {
    MapView* mapView = dynamic_cast<MapView*>(Toolkit::ToolResourceProvider::instance()->geoView());
    if (mapView)
      setContextLocation(mapView->screenToLocation(m_contextScreenPosition.x(), m_contextScreenPosition.y()));
  }

  // start tasks to determine whether a GeoElement was clicked on
  m_identifyGraphicsTask = geoView->identifyGraphicsOverlays(m_contextScreenPosition.x(), m_contextScreenPosition.y(), 5.0, false, 1);
  m_identifyFeaturesTask = geoView->identifyLayers(m_contextScreenPosition.x(), m_contextScreenPosition.y(), 5.0, false, 1);

  // accept the event to prevent it being used by other tools etc.
  event.accept();
}

/*!
  \internal.

  Handle the result of an identify layers task.
 */
void ContextMenuController::onIdentifyLayersCompleted(const QUuid& taskId, QList<IdentifyLayerResult*> identifyResults)
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

    const QList<GeoElement*> geoElements = res->geoElements();
    for(GeoElement* g : geoElements)
    {
      if (g == nullptr)
        continue;

      cancelIdentifyTasks();
      m_contextElement = g;
      m_contextElement->setParent(this);
      setResultTitle(res->layerContent()->name());
      addOption(IDENTIFY_OPTION);

      return;
    }
  }
}

/*!
  \internal.

  Handle the result of an identify graphics overlays task.
 */
void ContextMenuController::onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, QList<IdentifyGraphicsOverlayResult*> identifyResults)
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
    for(Graphic* g : graphics)
    {
      if (g == nullptr)
        continue;

      cancelIdentifyTasks();
      m_contextElement = g;
      m_contextElement->setParent(this);
      setResultTitle(res->graphicsOverlay()->overlayId());
      addOption(IDENTIFY_OPTION);
      addOption(FOLLOW_OPTION);

      return;
    }
  }
}


/*!
  \internal.

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
}

/*!
  \internal.

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
  \internal.

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
  \brief Returns the geographic location for the current context.
 */
Esri::ArcGISRuntime::Point ContextMenuController::contextLocation() const
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

    identifyTool->showPopup(m_contextElement, resultTitle());
  }
  else if (option == VIEWSHED_OPTION)
  {
    AnalysisController* analysisTool = Toolkit::ToolManager::instance().tool<AnalysisController>();
    if (!analysisTool)
      return;

    analysisTool->showMapPointViewshed(m_contextScreenPosition);
  }
  else if (option == FOLLOW_OPTION)
  {
    FollowPositionController* followTool = Toolkit::ToolManager::instance().tool<FollowPositionController>();
    if (!followTool)
      return;

    followTool->setFollowing(m_contextElement);
  }
  else if (option == COORDINATES_OPTION)
  {
    Toolkit::CoordinateConversionController* coordinateTool = Toolkit::ToolManager::instance().tool<Toolkit::CoordinateConversionController>();
    if (!coordinateTool)
      return;

    coordinateTool->setRunConversion(true);
    coordinateTool->setPointToConvert(m_contextLocation);
    coordinateTool->setActive(true);
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
bool ContextMenuController::contextActive() const
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
