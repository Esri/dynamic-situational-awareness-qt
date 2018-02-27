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
#include "IdentifyController.h"

#include "CoordinateConversionController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "MapView.h"
#include "SceneView.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

// RAII helper to ensure the QList<IdentifyLayerResult*> is deleted when we leave the scope
struct LayerResultsManager {

  QList<IdentifyLayerResult*>& m_results;

  LayerResultsManager(QList<IdentifyLayerResult*>& results):
    m_results(results)
  {
  }

  ~LayerResultsManager()
  {
    qDeleteAll(m_results);
  }
};

// RAII helper to ensure the QList<IdentifyGraphicsOverlayResult*> is deleted when we leave the scope
struct GraphicsOverlaysResultsManager {

  QList<IdentifyGraphicsOverlayResult*>& m_results;

  GraphicsOverlaysResultsManager(QList<IdentifyGraphicsOverlayResult*>& results):
    m_results(results)
  {
  }

  ~GraphicsOverlaysResultsManager()
  {
    qDeleteAll(m_results);
  }
};

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
  \brief Handles a mouse-click event in the view - used to trigger identify graphics and features tasks.
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

  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (sceneView)
    m_screenToLocationTask = sceneView->screenToLocation(m_contextScreenPosition.x(), m_contextScreenPosition.y());
  else
  {
    MapView* mapView = dynamic_cast<MapView*>(Toolkit::ToolResourceProvider::instance()->geoView());
    if (mapView)
      setContextLocation(mapView->screenToLocation(m_contextScreenPosition.x(), m_contextScreenPosition.y()));
  }

  m_identifyGraphicsTask = geoView->identifyGraphicsOverlays(m_contextScreenPosition.x(), m_contextScreenPosition.y(), 5.0, false, 1);
  m_identifyFeaturesTask = geoView->identifyLayers(m_contextScreenPosition.x(), m_contextScreenPosition.y(), 5.0, false, 1);

  // accept the event to prevent it being used by other tools etc.
  event.accept();
}

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

void ContextMenuController::onScreenToLocationCompleted(QUuid taskId, const Point& location)
{
  if (taskId != m_screenToLocationTask.taskId())
    return;

  m_screenToLocationTask = TaskWatcher();
  setContextLocation(location);
}

void ContextMenuController::setContextScreenPosition(const QPoint& contextScreenPosition)
{
  if (m_contextScreenPosition == contextScreenPosition)
    return;

  m_contextScreenPosition = contextScreenPosition;
  emit contextScreenPositionChanged();
}

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

void ContextMenuController::addOption(const QString &option)
{
  QStringList options = m_options->stringList();
  if (options.contains(option))
    return;

  options.append(option);
  m_options->setStringList(options);
  m_options->sort(0);

  setContextActive(true);
}

void ContextMenuController::clearOptions()
{
  m_options->setStringList(QStringList());
}

QString ContextMenuController::resultTitle() const
{
  return m_resultTitle;
}

void ContextMenuController::setResultTitle(const QString& resultTitle)
{
  if (m_resultTitle == resultTitle)
    return;

  m_resultTitle = resultTitle;
  emit resultTitleChanged();
}

void ContextMenuController::cancelTasks()
{
  cancelIdentifyTasks();
  m_screenToLocationTask.cancel();
  m_screenToLocationTask = TaskWatcher();
}

void ContextMenuController::cancelIdentifyTasks()
{
  m_identifyFeaturesTask.cancel();
  m_identifyFeaturesTask = TaskWatcher();
  m_identifyGraphicsTask.cancel();
  m_identifyGraphicsTask = TaskWatcher();
}

QString ContextMenuController::result() const
{
  return m_result;
}

void ContextMenuController::setResult(const QString& result)
{
  m_result = result;
  emit resultChanged();
}

QStringListModel* ContextMenuController::options() const
{
  return m_options;
}

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

QPoint ContextMenuController::contextScreenPosition() const
{
  return m_contextScreenPosition;
}

bool ContextMenuController::contextActive() const
{
  return m_contextActive;
}

void ContextMenuController::setContextActive(bool contextRequested)
{
  if (m_contextActive == contextRequested)
    return;

  m_contextActive = contextRequested;
  emit contextActiveChanged();
}
