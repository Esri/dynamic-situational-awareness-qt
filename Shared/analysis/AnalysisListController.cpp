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

#include "AnalysisListController.h"
#include "CombinedAnalysisListModel.h"
#include "ViewshedController.h"
#include "LineOfSightController.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "AnalysisOverlayListModel.h"
#include "SceneView.h"

using namespace Esri::ArcGISRuntime;

AnalysisListController::AnalysisListController(QObject* parent):
  Toolkit::AbstractTool(parent),
  m_analysisList(new CombinedAnalysisListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]()
  {
    onGeoViewChanged(Toolkit::ToolResourceProvider::instance()->geoView());
  });

  onGeoViewChanged(Toolkit::ToolResourceProvider::instance()->geoView());
}

AnalysisListController::~AnalysisListController()
{
}

QString AnalysisListController::toolName() const
{
  return QStringLiteral("Analysis List");
}

QAbstractItemModel* AnalysisListController::analysisList() const
{
  return m_analysisList;
}

void AnalysisListController::removeAt(int index)
{
  m_analysisList->removeAt(index);
}

void AnalysisListController::zoomTo(int index)
{
  zoomToLocation(m_analysisList->locationAt(index));
}

void AnalysisListController::onGeoViewChanged(GeoView* geoView)
{
  if (geoView == nullptr)
    return;

  SceneView* sceneView = dynamic_cast<SceneView*>(geoView);
  if (!sceneView)
    return;

  m_sceneView = sceneView;

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

  connect(m_sceneView->analysisOverlays(), &AnalysisOverlayListModel::analysisOverlayAdded, this, handleAnalysisOverlaysChanged);
  handleAnalysisOverlaysChanged();
}

void AnalysisListController::zoomToLocation(const Point& point)
{
  if (m_sceneView == nullptr)
    return;

  const Camera currentCam = m_sceneView->currentViewpointCamera();
  constexpr double targetDistance = 1500.0;
  const Camera newCam(point, targetDistance, currentCam.heading(), currentCam.pitch(), currentCam.roll());

  m_sceneView->setViewpointCamera(newCam, 1.0);
}
