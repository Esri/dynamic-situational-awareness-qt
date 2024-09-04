
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

#include "AnalysisListController.h"

// dsa app headers
#include "CombinedAnalysisListModel.h"
#include "LineOfSightController.h"
#include "ViewshedController.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "Camera.h"
#include "SceneView.h"

// Qt headers
#include <QFuture>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::AnalysisListController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for working with the list of Analysis objects.

  Analyses are the result of either a viewshed or line of sight operation.

  This tool presents the list of currently active analyses and allows these to
  be removed, set visible etc.

  \sa LineOfSightController
  \sa ViewshedController
  */

/*!
  \brief Constructor accepting an optional \a parent.
 */
AnalysisListController::AnalysisListController(QObject* parent):
  AbstractTool(parent),
  m_analysisList(new CombinedAnalysisListModel(this))
{
  // update the geoView used by the tool as required
  connect(ToolResourceProvider::instance(), &ToolResourceProvider::geoViewChanged, this, [this]()
  {
    onGeoViewChanged(ToolResourceProvider::instance()->geoView());
  });

  onGeoViewChanged(ToolResourceProvider::instance()->geoView());

  ToolManager::instance().addTool(this);
}

/*!
  \brief Destructor.
 */
AnalysisListController::~AnalysisListController()
{
}

/*!
  \brief Returns the name of this tool.
 */
QString AnalysisListController::toolName() const
{
  return QStringLiteral("Analysis List");
}

/*!
  \property ViewedAlertsController::analysisList
  \brief Returns the list of analyses.
 */
QAbstractItemModel* AnalysisListController::analysisList() const
{
  return m_analysisList;
}

/*!
  \brief Removes the analysis at \a index from the list.
 */
void AnalysisListController::removeAt(int index)
{
  m_analysisList->removeAt(index);
}

/*!
  \brief Zooms the geoView to the locaction of the analysis at \a index in the list.
 */
void AnalysisListController::zoomTo(int index)
{
  zoomToLocation(m_analysisList->locationAt(index));
}

/*!
  \brief Updates the geoView in use by the tool to \a geoView.

  \note Since this tool manages the list of scene analysis, the geoView
  will only used if it is a \l Esri::ArcGISRuntime::SceneView.
 */
void AnalysisListController::onGeoViewChanged(GeoView* geoView)
{
  if (geoView == nullptr)
    return;

  SceneView* sceneView = dynamic_cast<SceneView*>(geoView);
  if (!sceneView)
    return;

  m_sceneView = sceneView;

  // handle changes to the list of analysis overlays used by the view
  auto handleAnalysisOverlaysChanged = [this]()
  {
    ViewshedController* viewshed = ToolManager::instance().tool<ViewshedController>();
    if (viewshed != nullptr)
      m_analysisList->setViewshedModel(viewshed->viewsheds());

    LineOfSightController* lineOfSight = ToolManager::instance().tool<LineOfSightController>();
    if (lineOfSight != nullptr)
      m_analysisList->setLineOfSightModel(lineOfSight->lineOfSightOverlay()->analyses());

    emit analysisListChanged();
  };

  // react to changes in the sceneView's analysis overlays list
  connect(m_sceneView->analysisOverlays(), &AnalysisOverlayListModel::itemAdded, this, handleAnalysisOverlaysChanged);
  handleAnalysisOverlaysChanged();
}

/*!
  \internal
 */
void AnalysisListController::zoomToLocation(const Point& point)
{
  if (m_sceneView == nullptr)
    return;

  // create a new Camera (using the existing heading, pitch etc.) focused on the supplied point
  const Camera currentCam = m_sceneView->currentViewpointCamera();
  constexpr double targetDistance = 1500.0;
  const Camera newCam(point, targetDistance, currentCam.heading(), currentCam.pitch(), currentCam.roll());

  m_sceneView->setViewpointCameraAsync(newCam, 1.0);
}

} // Dsa

// Signal Documentation
/*!
  \fn void AnalysisListController::analysisListChanged();
  \brief Signal emitted when the analysis list changes.
 */
