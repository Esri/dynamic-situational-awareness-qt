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

#include "AnalysisListController.h"

// example app headers
#include "CombinedAnalysisListModel.h"
#include "LineOfSightController.h"
#include "ViewshedController.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "AnalysisOverlayListModel.h"
#include "SceneView.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class AnalysisListController
  \inherits Toolkit::AbstractTool
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
  Toolkit::AbstractTool(parent),
  m_analysisList(new CombinedAnalysisListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  // update the geoView used by the tool as required
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]()
  {
    onGeoViewChanged(Toolkit::ToolResourceProvider::instance()->geoView());
  });

  onGeoViewChanged(Toolkit::ToolResourceProvider::instance()->geoView());
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
    ViewshedController* viewshed = Toolkit::ToolManager::instance().tool<ViewshedController>();
    if (viewshed != nullptr)
      m_analysisList->setViewshedModel(viewshed->viewsheds());

    LineOfSightController* lineOfSight = Toolkit::ToolManager::instance().tool<LineOfSightController>();
    if (lineOfSight != nullptr)
      m_analysisList->setLineOfSightModel(lineOfSight->lineOfSightOverlay()->analyses());

    emit analysisListChanged();
  };

  // react to changes in the sceneView's analysis overlays list
  connect(m_sceneView->analysisOverlays(), &AnalysisOverlayListModel::analysisOverlayAdded, this, handleAnalysisOverlaysChanged);
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

  m_sceneView->setViewpointCamera(newCam, 1.0);
}

} // Dsa

// Signal Documentation
/*!
  \fn void AnalysisListController::analysisListChanged();
  \brief Signal emitted when the analysis list changes.
 */
