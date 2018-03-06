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

#include "AnalysisListController.h"
#include "CombinedAnalysisListModel.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "AnalysisOverlayListModel.h"
#include "GeoElementLineOfSight.h"
#include "GeoElementViewshed.h"
#include "LocationLineOfSight.h"
#include "LocationViewshed.h"
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
  Analysis* analysis = m_analysisList->analysisAt(index);
  if (analysis == nullptr)
    return;

  AnalysisOverlay* overlay = m_analysisList->overlayAt(index);
  if (overlay == nullptr)
    return;

  overlay->analyses()->removeOne(analysis);
  analysis->deleteLater();
}

void AnalysisListController::zoomTo(int index)
{
  Analysis* analysis = m_analysisList->analysisAt(index);
  if (analysis == nullptr)
    return;

  switch (analysis->analysisType())
  {
  case AnalysisType::LocationViewshed:
  {
    LocationViewshed* locationViewshed = qobject_cast<LocationViewshed*>(analysis);
    if (locationViewshed == nullptr)
      return;

    zoomToLocation(locationViewshed->location());
    break;
  }
  case AnalysisType::LocationLineOfSight:
  {
    LocationLineOfSight* locationLineOfSight = qobject_cast<LocationLineOfSight*>(analysis);
    if (locationLineOfSight == nullptr)
      return;

    zoomToLocation(locationLineOfSight->targetLocation());
    break;
  }
  case AnalysisType::GeoElementViewshed:
  {
    GeoElementViewshed* geoElementViewshed = qobject_cast<GeoElementViewshed*>(analysis);
    if (geoElementViewshed == nullptr)
      return;

    zoomToLocation(geoElementViewshed->geoElement()->geometry().extent().center());
    break;
  }
  case AnalysisType::GeoElementLineOfSight:
  {
    GeoElementLineOfSight* geoElementLineOfSight = qobject_cast<GeoElementLineOfSight*>(analysis);
    if (geoElementLineOfSight == nullptr)
      return;

    zoomToLocation(geoElementLineOfSight->targetGeoElement()->geometry().extent().center());
    break;
  }
  default:
    return;
  }
}

void AnalysisListController::onGeoViewChanged(GeoView* geoView)
{
  if (!geoView)
    return;

  SceneView* sceneView = dynamic_cast<SceneView*>(geoView);
  if (!sceneView)
    return;

  m_sceneView = sceneView;
  m_analysisList->setSceneView(sceneView);

  emit analysisListChanged();
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
