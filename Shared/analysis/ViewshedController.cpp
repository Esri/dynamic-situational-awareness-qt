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

#include "ViewshedController.h"
#include "GraphicsOverlaysResultsManager.h"
#include "LocationController.h"
#include "LocationDisplay3d.h"
#include "PointViewshed.h"
#include "GraphicViewshed.h"
#include "ViewshedListModel.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "SceneQuickView.h"
#include "LocationViewshed.h"
#include "GeoElementViewshed.h"

using namespace Esri::ArcGISRuntime;

static const QString s_headingAttribute{ QStringLiteral("heading") };
static const QString s_pitchAttribute{ QStringLiteral("pitch") };
static int s_viewshedCount = 0;

ViewshedController::ViewshedController(QObject *parent) :
  Toolkit::AbstractTool(parent),
  m_analysisOverlay(new AnalysisOverlay(this)),
  m_viewsheds(new ViewshedListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]
  {
    updateGeoView();
  });

  connectMouseSignals();

  updateGeoView();

  connect(m_viewsheds, &ViewshedListModel::viewshedRemoved, this, [this](AbstractViewshed* viewshed)
  {
    viewshed->removeFromOverlay();

    if (viewshed == m_locationDisplayViewshed)
    {
      m_locationDisplayViewshed = nullptr;

      emit locationDisplayViewshedActiveChanged();
    }

    delete viewshed;
  });
}

ViewshedController::~ViewshedController()
{
}

void ViewshedController::updateGeoView()
{
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  if (!sceneView->analysisOverlays()->contains(m_analysisOverlay))
    sceneView->analysisOverlays()->append(m_analysisOverlay);
}

void ViewshedController::connectMouseSignals()
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked, this, [this](QMouseEvent& event)
  {
    if (!isActive())
      return;

    SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
    if (!sceneView)
      return;

    switch (m_activeMode)
    {
    case AddMapPointViewshed:
    {
      const Point pt = sceneView->screenToBaseSurface(event.x(), event.y());
      addMapPointViewshed(pt);
      break;
    }
    case AddMessageFeedViewshed:
    {
      if (!m_identifyConn)
      {
        m_identifyConn = connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyGraphicsOverlaysCompleted,
                                 this, [this](const QUuid& taskId, const QList<IdentifyGraphicsOverlayResult*>& identifyResults)
        {
          if (taskId != m_identifyTaskWatcher.taskId())
            return;

          m_identifyTaskWatcher = TaskWatcher();

          if (!isActive() || identifyResults.isEmpty() || identifyResults[0]->graphics().isEmpty())
          {
            qDeleteAll(identifyResults); // TODO: should use the GraphicsOverlaysResultsManager from the IdentifyController instead
            return;
          }

          addMessageFeedViewshed(identifyResults[0]->graphics()[0]);

          qDeleteAll(identifyResults); // TODO: should use the GraphicsOverlaysResultsManager from the IdentifyController instead
        });
      }

      m_identifyTaskWatcher = Toolkit::ToolResourceProvider::instance()->geoView()->identifyGraphicsOverlays(event.x(), event.y(), 5.0, false, 1);
      break;
    }
    default:
      break;
    }
  });
}

void ViewshedController::addLocationDisplayViewshed()
{
  if (m_locationDisplayViewshed)
    return;

  LocationController* locationController = Toolkit::ToolManager::instance().tool<LocationController>();
  if (!locationController)
    return;

  Graphic* locationGraphic = locationController->locationDisplay()->locationGraphic();
  m_locationDisplayViewshed = new GraphicViewshed(locationGraphic, m_analysisOverlay, s_headingAttribute, s_pitchAttribute, this);
  m_locationDisplayViewshed->setName(QStringLiteral("Location Display Viewshed"));
  m_locationDisplayViewshed->setOffsetZ(-5.0);
  m_analysisOverlay->analyses()->append(m_locationDisplayViewshed->viewshed());
  m_viewsheds->append(m_locationDisplayViewshed);

  emit locationDisplayViewshedActiveChanged();
}

void ViewshedController::addMapPointViewshed(const Esri::ArcGISRuntime::Point& point)
{
  if (!m_graphicsOverlay)
  {
    SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
    if (!sceneView)
      return;

    m_graphicsOverlay = new GraphicsOverlay(this);
    m_graphicsOverlay->setOverlayId("Map point analysis overlay");
    sceneView->graphicsOverlays()->append(m_graphicsOverlay);
  }

  auto pointViewshed = new PointViewshed(point, m_graphicsOverlay, m_analysisOverlay, this);
  s_viewshedCount++;
  pointViewshed->setName(QString("Viewshed %1").arg(QString::number(s_viewshedCount)));
  m_analysisOverlay->analyses()->append(pointViewshed->viewshed());
  m_viewsheds->append(pointViewshed);
}

void ViewshedController::addMessageFeedViewshed(Graphic* graphic)
{
  auto messageFeedViewshed = new GraphicViewshed(graphic, m_analysisOverlay, QString(), QString(), this);
  s_viewshedCount++;
  messageFeedViewshed->setName(QString("Viewshed %1").arg(QString::number(s_viewshedCount)));
  graphic->setParent(messageFeedViewshed);

  messageFeedViewshed->setOffsetZ(5.0);
  m_analysisOverlay->analyses()->append(messageFeedViewshed->viewshed());
  m_viewsheds->append(messageFeedViewshed);
}

bool ViewshedController::isLocationDisplayViewshedActive() const
{
  return m_locationDisplayViewshed != nullptr;
}

ViewshedController::ViewshedActiveMode ViewshedController::activeMode() const
{
  return m_activeMode;
}

void ViewshedController::setActiveMode(ViewshedActiveMode mode)
{
  if (m_activeMode == mode)
    return;

  m_activeMode = mode;

  emit activeModeChanged();
}

ViewshedListModel* ViewshedController::viewsheds() const
{
  return m_viewsheds;
}

QString ViewshedController::toolName() const
{
  return QStringLiteral("viewshed");
}
