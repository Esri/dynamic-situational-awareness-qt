
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

#include "IdentifyController.h"

// dsa app headers
#include "GraphicsOverlaysResultsManager.h"
#include "LayerResultsManager.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "AttributeListModel.h"
#include "GeoElement.h"
#include "GeoView.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "IdentifyLayerResult.h"
#include "LayerContent.h"
#include "Popup.h"
#include "PopupAttributeListModel.h"
#include "PopupDefinition.h"
#include "PopupField.h"
#include "PopupFieldFormat.h"
#include "PopupManager.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::IdentifyController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for identifying GeoElements.
 */

/*!
  \brief Constructor accepting an optional \a parent.
 */
IdentifyController::IdentifyController(QObject* parent /* = nullptr */):
  AbstractTool(parent)
{
  // setup connection to handle mouse-clicking in the view (used to trigger the identify tasks)
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::mouseClicked,
          this, &IdentifyController::onMouseClicked);

  // setup connection to handle the results of an Identify Layers task
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::identifyLayersCompleted,
          this, &IdentifyController::onIdentifyLayersCompleted);

  // setup connection to handle the results of an Identify Graphic Overlays task
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::identifyGraphicsOverlaysCompleted,
          this, &IdentifyController::onIdentifyGraphicsOverlaysCompleted);

  ToolManager::instance().addTool(this);
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
  \property IdentifyController::busy
  \brief Returns whether the tool is busy or not (e.g. whether identify tasks are running).
 */
bool IdentifyController::busy() const
{
  return (m_layersWatcher.isValid() && !m_layersWatcher.isDone() && !m_layersWatcher.isCanceled()) ||
      (m_graphicsOverlaysWatcher.isValid() && !m_graphicsOverlaysWatcher.isDone() && !m_graphicsOverlaysWatcher.isCanceled());
}

/*!
  \property IdentifyController::popupManagers
  \brief Returns a QVariantList of \l Esri::ArcGISRuntime::PopupManager which can be displayed in the view.

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
  \brief Show the popup for \a geoElement with the title \a popupTitle.
 */
void IdentifyController::showPopup(GeoElement* geoElement, const QString& popupTitle)
{
  if (!geoElement)
    return;

  m_popupManagers.clear();
  emit popupManagersChanged();
  addGeoElementPopup(geoElement, popupTitle);
  emit popupManagersChanged();
}

/*!
  \brief Show popups for all of the \a geoElementsByTitle.

  A popup will be created for each \l Esri::ArcGISRuntime::GeoElement in the QHash,
  with the string key as the title.
 */
void IdentifyController::showPopups(const QHash<QString, QList<GeoElement*>>& geoElementsByTitle)
{
  if (geoElementsByTitle.isEmpty())
    return;

  m_popupManagers.clear();
  emit popupManagersChanged();

  for (auto it = geoElementsByTitle.cbegin(); it != geoElementsByTitle.cend(); ++it)
  {
    const QString& popupTitle = it.key();
    for (GeoElement* geoElement : qAsConst(it.value()))
      addGeoElementPopup(geoElement, popupTitle);
  }

  emit popupManagersChanged();
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

  GeoView* geoView = ToolResourceProvider::instance()->geoView();
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

  Creates a new \l Esri::ArcGISRuntime::PopupManager objects for every valid feature with attributes
 */
void IdentifyController::onIdentifyLayersCompleted(const QUuid& taskId, QList<IdentifyLayerResult*> identifyResults)
{
  // if the task Id does not match the one we are tracking, ignore it
  if (taskId != m_layersWatcher.taskId())
    return;

  // Create a RAII helper to ensure we clean up the results
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

  // Create a RAII helper to ensure we clean up the results
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
  \brief Helper method to create a new PopupManager with the title \a popupTitle,
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

  for (auto popupfield : newManager->displayedFields()->popupFields())
  {
    if (!popupfield->format())
    {
      auto format = new PopupFieldFormat(newManager);
      format->setDecimalPlaces(2);
      format->setUseThousandsSeparator(true);
      popupfield->setFormat(format);
    }
  }

  m_popupManagers.push_back(newManager);

  return true;
}

} // Dsa

// Signal Documentation
/*!
  \fn void IdentifyController::busyChanged();
  \brief Signal emitted when the busy property changes.
 */

/*!
  \fn void IdentifyController::popupManagersChanged();
  \brief Signal emitted when the popup managers change.
 */

