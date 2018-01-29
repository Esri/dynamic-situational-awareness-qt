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

#include "IdentifyController.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "GeoElement.h"
#include "GeoView.h"
#include "Graphic.h"
#include "Popup.h"
#include "PopupManager.h"

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

/*!
  \brief Constructor accepting an optional \a parent.
 */
IdentifyController::IdentifyController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  // setup connection to handle mouse-clicking in the view (used to trigger the identify tasks)
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked,
          this, &IdentifyController::onMouseClicked);

  // setup connection to handle the results of an Identify Layers task
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyLayersCompleted,
          this, &IdentifyController::onIdentifyLayersCompleted);

  // setup connection to handle the results of an Identify Graphic Overlays task
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyGraphicsOverlaysCompleted,
          this, &IdentifyController::onIdentifyGraphicsOverlaysCompleted);
}

/*!
  \brief Destructor.
 */
IdentifyController::~IdentifyController()
{
}

/*!
  \brief The name of this tool.
 */
QString IdentifyController::toolName() const
{
  return QStringLiteral("identify");
}

/*!
  \brief Sets whether this tool should be \a active or not.

  When active, the tool will kick off identify tasks for graphics and feataures when
  it recieves a mouse-clcik in the view.
 */
void IdentifyController::setActive(bool active)
{
  if (active == m_active)
    return;

  // if the tool is busy (identify tasks are in-progress), cancel those tasks and start new ones
  if (busy())
  {
    m_layersWatcher.cancel();
    m_graphicsOverlaysWatcher.cancel();
  }

  m_active = active;
  emit activeChanged();
}

/*!
  \brief Returns whether the tool is busy or not (e.g. whether identify tasks are running).
 */
bool IdentifyController::busy() const
{
  return (m_layersWatcher.isValid() && !m_layersWatcher.isDone() && !m_layersWatcher.isCanceled()) ||
      (m_graphicsOverlaysWatcher.isValid() && !m_graphicsOverlaysWatcher.isDone() && !m_graphicsOverlaysWatcher.isCanceled());
}

/*!
  \brief Returns a QVariantList of \l ESri::ArcGISRuntime::PopupManager which can be displayed in the view.

  For example, this can be passed to a \l PopupView or \l PopupStackView for display.
 */
QVariantList IdentifyController::popupManagers() const
{
  QVariantList res;

  for (PopupManager* mgr : m_popupManagers)
  {
    QVariant v = QVariant::fromValue(mgr);
    res.push_back(v);
  }

  return res;
}

/*!
  \brief Handles a mouse-click event in the view - used to trigger identify graphics and features tasks.
 */
void IdentifyController::onMouseClicked(QMouseEvent& event)
{
  // ignore the event if the tool is not active.
  if (!isActive())
    return;

  // only consider left clicks (or taps) onn the view.
  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  // Ignore the event if the tool is cuurrently running tasks.
  if (busy())
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  // start new identifyLayers and identifyGraphicsOverlays tasks at the x and y position of the event and using the
  // specifed tolerance (m_tolerance) to determine how accurate a hit-test to perform.
  // create a TaskWatcher to store the progress/state of the task.
  m_layersWatcher = geoView->identifyLayers(event.pos().x(), event.pos().y(), m_tolerance, false);
  m_graphicsOverlaysWatcher = geoView->identifyGraphicsOverlays(event.pos().x(), event.pos().y(), m_tolerance, false);
  emit busyChanged();

  m_popupManagers.clear();
  emit popupManagersChanged();

  // accept the event to prevent it being used by other tools etc.
  event.accept();
}

/*!
  \brief Handles the output of an IdentifyLayers task with Id \a taskId and results \l identifyResults.

  Creates a new \l Esri::ArcGISRumtime::PopupManager objects for every valid feature with attributes
 */
void IdentifyController::onIdentifyLayersCompleted(const QUuid& taskId, QList<IdentifyLayerResult*> identifyResults)
{
  // if the task Id does not match the one we are tracking, ignore it
  if (taskId != m_layersWatcher.taskId())
    return;

  // Create a RAII helper to ensure we clean uop the results
  LayerResultsManager resultsManager(identifyResults);

  m_layersWatcher = TaskWatcher();
  emit busyChanged();

  if (!isActive())
    return;

  // iterate over the results and add a new PopupManager for any valid features, with attributes
  bool anyAdded = false;
  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyLayerResult* res = *it;
    if (!res)
      continue;

    const QString resTitle = res->layerContent()->name();
    const QList<GeoElement*> geoElements = res->geoElements();
    for(GeoElement* g : geoElements)
    {
      if (addGeoElementPopup(g, resTitle))
        anyAdded = true;
    }
  }

  if (anyAdded)
    emit popupManagersChanged();
}

/*!
  \brief Handles the output of an IdentifyGraphicsOverlays task with Id \a taskId and results \l identifyResults.

  Creates a new \l Esri::ArcGISRumtime::PopupManager objects for every valid graphic with attributes
 */
void IdentifyController::onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, QList<IdentifyGraphicsOverlayResult*> identifyResults)
{
  // if the task Id does not match the one we are tracking, ignore it
  if (taskId != m_graphicsOverlaysWatcher.taskId())
    return;

  // Create a RAII helper to ensure we clean uop the results
  GraphicsOverlaysResultsManager resultsManager(identifyResults);

  m_graphicsOverlaysWatcher = TaskWatcher();
  emit busyChanged();

  if (!isActive())
    return;

  // iterate over the results and add a new PopupManager for any valid graphics, with attributes
  bool anyAdded = false;
  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyGraphicsOverlayResult* res = *it;
    if (!res)
      continue;

    const QString resTitle = res->graphicsOverlay()->overlayId();
    const QList<Graphic*> graphics = res->graphics();

    for(Graphic* g : graphics)
    {
      if (addGeoElementPopup(g, resTitle))
        anyAdded = true;
    }
  }

  if (anyAdded)
    emit popupManagersChanged();
}

/*!
  \brief Helper method to create a new PopupManager with the totlel \a popupTitle,
  if \a geoElement is valid and has attributes.
 */
bool IdentifyController::addGeoElementPopup(GeoElement* geoElement, const QString& popupTitle)
{
  if (!geoElement)
    return false;

  if (!geoElement->attributes() || geoElement->attributes()->isEmpty())
    return false;

  // create a new Popup from the geoElement
  Popup* newPopup = new Popup(geoElement, this);
  newPopup->popupDefinition()->setTitle(popupTitle);
  PopupManager* newManager = new PopupManager(newPopup, this);

  m_popupManagers.push_back(newManager);

  return true;
}
