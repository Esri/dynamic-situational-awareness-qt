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

// DSA headers
#include "GraphicsOverlaysResultsManager.h"
#include "LayerResultsManager.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

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

  m_active = active;
  emit activeChanged();
}

/*!
  \property IdentifyController::busy
  \brief Returns whether the tool is busy or not (e.g. whether identify tasks are running).
 */
bool IdentifyController::busy() const
{
  return m_isBusy;
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

  // Ignore the event if the tool is currently running tasks.
  if (busy())
    return;

  auto* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  // start new identifyLayers and identifyGraphicsOverlays tasks at the x and y position of the event and using the
  // specifed tolerance (m_tolerance) to determine how accurate a hit-test to perform.
  // invoke the identify operations on the geoview for layers and graphics overlays
  auto identify_layers = geoView->identifyLayersAsync(event.position(), m_tolerance, false, -1, this);
  auto identify_graphics = geoView->identifyGraphicsOverlaysAsync(event.position(), m_tolerance, false, -1, this);

  // respond once all QFutures are complete (includes any cancel or failure as well)
  QtFuture::whenAll(identify_layers, identify_graphics).then(this, [this](const QList<IdentifyResultsVariant::FutureType> &identify_results)
  {
    m_isBusy = true;
    emit busyChanged();
    // abort if tool is no longer the active tool
    if (!isActive())
      return;

    // iterate over each type in the results variant
    bool anyAdded = false;
    for (const IdentifyResultsVariant::FutureType& identify_result : identify_results)
    {
      if (identify_result.index() == IdentifyResultsVariant::Types::LAYERS)
      {
        LayerResultsManager resultsManager(std::get<IdentifyResultsVariant::Types::LAYERS>(identify_result).result());
        for (auto* result : resultsManager.m_results)
        {
          if (!result)
            continue;

          const auto& layerName = result->layerContent()->name();
          for (auto* geoElement : result->geoElements())
          {
            if (addGeoElementPopup(geoElement, layerName))
              anyAdded = true;
          }
        }
      }
      else if (identify_result.index() == IdentifyResultsVariant::Types::GRAPHICS)
      {
        GraphicsOverlaysResultsManager resultsManager(std::get<IdentifyResultsVariant::Types::GRAPHICS>(identify_result).result());
        for (auto* result : resultsManager.m_results)
        {
          if (!result)
            continue;

          const auto& layerName = result->graphicsOverlay()->overlayId();
          for (auto* graphic : result->graphics())
          {
            if (!addGeoElementPopup(graphic, layerName))
              anyAdded = true;
          }
        }
      }
    }

    m_isBusy = false;
    emit busyChanged();

    // emit popup manager changed signal when anyresults are found
    if (anyAdded)
      emit popupManagersChanged();
  });

  emit busyChanged();

  m_popupManagers.clear();
  emit popupManagersChanged();

  // accept the event to prevent it being used by other tools etc.
  event.accept();
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

