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
#include "ToolManager.h"
#include "ToolResourceProvider.h"
#include "SceneQuickView.h"
#include "LocationViewshed.h"
#include "GeoElementViewshed.h"
#include "LocationController.h"
#include "LocationDisplay3d.h"
#include "PointViewshed.h"
#include "GraphicViewshed.h"
#include "ViewshedListModel.h"

using namespace Esri::ArcGISRuntime;

//const QString AnalysisController::MAP_POINT_VIEWSHED_TYPE = QStringLiteral("Map Point");
//const QString AnalysisController::MY_LOCATION_VIEWSHED_TYPE = QStringLiteral("My Location");
//const QString AnalysisController::FRIENDLY_TRACK_VIEWSHED_TYPE = QStringLiteral("Friendly Track");

static const QString s_headingAttribute{ QStringLiteral("heading") };
static const QString s_pitchAttribute{ QStringLiteral("pitch") };

AnalysisController::AnalysisController(QObject *parent) :
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
    if (!viewshed)
      return;

    if (viewshed == m_locationDisplayViewshed)
    {
      m_locationDisplayViewshed = nullptr;
      emit locationDisplayViewshedActiveChanged();
    }

    m_analysisOverlay->analyses()->removeOne(viewshed->viewshed());

    delete viewshed;
  });
}

AnalysisController::~AnalysisController()
{
}

void AnalysisController::updateGeoView()
{
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  if (!sceneView->analysisOverlays()->contains(m_analysisOverlay))
    sceneView->analysisOverlays()->append(m_analysisOverlay);
}

void AnalysisController::connectMouseSignals()
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked, this, [this](QMouseEvent& event)
  {
    if (!isActive())
      return;

    if (!m_viewshedEnabled)
      return;

    switch (m_activeMode)
    {
    case AddMapPointViewshed:
      addMapPointViewshed(event);
      break;
    case AddMessageFeedViewshed:
      addMessageFeedViewshed(event);
      break;
    case EditMapPointViewshed:
      break;
    default:
      break;
    }
  });
}

//void AnalysisController::updateCurrentViewshed()
//{
//  auto viewshed = m_viewsheds[m_viewshedTypeIndex];
//  if (m_currentViewshed && m_currentViewshed == viewshed)
//    return;

//  m_currentViewshed = viewshed;

//  if (m_viewshedTypeIndex == 1)
//    updateMyLocationViewshed();

//  emitAllChanged();
//}

void AnalysisController::addMapPointViewshed(QMouseEvent& event)
{
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  if (!m_graphicsOverlay)
  {
    m_graphicsOverlay = new GraphicsOverlay(this);
    m_graphicsOverlay->setOverlayId("Map point analysis overlay");
    sceneView->graphicsOverlays()->append(m_graphicsOverlay);
  }

  Point pt = sceneView->screenToBaseSurface(event.x(), event.y());

  auto pointViewshed = new PointViewshed(pt, m_graphicsOverlay, m_analysisOverlay, this);
  pointViewshed->setName(QString("Viewshed %1").arg(QString::number(m_viewsheds->count() + 1)));
  m_viewsheds->append(pointViewshed);
  m_analysisOverlay->analyses()->append(pointViewshed->viewshed());

//  if (!m_currentViewshed)
//    return;

//  const Point pt = sceneView->screenToBaseSurface(event.x(), event.y());
//  static_cast<PointViewshed*>(m_currentViewshed)->setPoint(pt);
}

void AnalysisController::addLocationDisplayViewshed()
{
  if (m_locationDisplayViewshed)
    return;

  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  LocationController* locationController = Toolkit::ToolManager::instance().tool<LocationController>();
  if (locationController)
  {
    Graphic* locationGraphic = locationController->locationDisplay()->locationGraphic();
    m_locationDisplayViewshed = new GraphicViewshed(locationGraphic, m_analysisOverlay, s_headingAttribute, s_pitchAttribute, this);
    m_locationDisplayViewshed->setName(QString("Viewshed %1").arg(QString::number(m_viewsheds->count() + 1)));
    m_locationDisplayViewshed->setOffsetZ(-5.0);
    m_viewsheds->append(m_locationDisplayViewshed);
    m_analysisOverlay->analyses()->append(m_locationDisplayViewshed->viewshed());

    emit locationDisplayViewshedActiveChanged();
  }
}

void AnalysisController::removeLocationDisplayViewshed()
{
  if (!m_locationDisplayViewshed)
    return;

  m_viewsheds->removeOne(m_locationDisplayViewshed);
}

void AnalysisController::addMessageFeedViewshed(QMouseEvent& event)
{
  if (!m_identifyConn)
  {
    m_identifyConn = connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyGraphicsOverlaysCompleted,
                             this, [this](const QUuid& taskId, const QList<IdentifyGraphicsOverlayResult*>& identifyResults)
    {
      if (taskId != m_identifyTaskWatcher.taskId())
        return;

      m_identifyTaskWatcher = TaskWatcher();

      if (!m_viewshedEnabled || identifyResults.isEmpty() || identifyResults[0]->graphics().isEmpty())
      {
        qDeleteAll(identifyResults); // TODO: should use the GraphicsOverlaysResultsManager from the IdentifyController instead
        return;
      }

      Graphic* graphic = identifyResults[0]->graphics()[0];

//      if (m_currentViewshed)
//      {
//        auto geoElementViewshed = dynamic_cast<GeoElementViewshed*>(m_currentViewshed->viewshed());
//        if (geoElementViewshed && qobject_cast<Graphic*>(geoElementViewshed->geoElement()) == graphic)
//        {
//          qDeleteAll(identifyResults); // TODO: should use the GraphicsOverlaysResultsManager from the IdentifyController instead
//          return;
//        }

//        removeViewshed();
//      }

//      auto geoElementViewshed = new GeoElementViewshed(graphic, m_horizontalAngleDefault, m_verticalAngleDefault,
//                                                       m_minDistanceDefault, m_maxDistanceDefault,
//                                                       m_headingOffsetDefault, m_pitchOffsetDefault, this);
      auto messageFeedViewshed = new GraphicViewshed(graphic, m_analysisOverlay, QString(), QString(), this);
      messageFeedViewshed->setName(QString("Viewshed %1").arg(QString::number(m_viewsheds->count() + 1)));
      graphic->setParent(messageFeedViewshed);

      messageFeedViewshed->setOffsetZ(5.0);
      m_viewsheds->append(messageFeedViewshed);
      m_analysisOverlay->analyses()->append(messageFeedViewshed->viewshed());

      qDeleteAll(identifyResults); // TODO: should use the GraphicsOverlaysResultsManager from the IdentifyController instead
    });
  }

  m_identifyTaskWatcher = Toolkit::ToolResourceProvider::instance()->geoView()->identifyGraphicsOverlays(event.x(), event.y(), 5.0, false, 1);
}

//void AnalysisController::removeViewshed()
//{
//  if (!m_currentViewshed)
//    return;

//  m_analysisOverlay->analyses()->removeOne(m_currentViewshed->viewshed());

//  delete m_currentViewshed;
//  m_currentViewshed = nullptr;
//  m_viewsheds[m_viewshedTypeIndex] = nullptr;

//  emitAllChanged();
//}

bool AnalysisController::isViewshedEnabled() const
{
  return m_viewshedEnabled;
}

void AnalysisController::setViewshedEnabled(bool viewshedEnabled)
{
  if (m_viewshedEnabled == viewshedEnabled)
    return;

  m_viewshedEnabled = viewshedEnabled;

  emit viewshedEnabledChanged();
}

bool AnalysisController::isLocationDisplayViewshedActive() const
{
  return m_locationDisplayViewshed != nullptr;
}

AnalysisController::AnalysisActiveMode AnalysisController::analysisActiveMode() const
{
  return m_activeMode;
}

void AnalysisController::setAnalysisActiveMode(AnalysisActiveMode mode)
{
  if (m_activeMode == mode)
    return;

  m_activeMode = mode;

  emit analysisActiveModeChanged();
}

QAbstractListModel* AnalysisController::viewsheds() const
{
  return m_viewsheds;
}

//bool AnalysisController::isViewshedVisible() const
//{
//  return m_currentViewshed ? m_currentViewshed->isVisible() : false;
//}

//void AnalysisController::setViewshedVisible(bool viewshedVisible)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->isVisible() == viewshedVisible)
//    return;

//  m_currentViewshed->setVisible(viewshedVisible);

//  emit viewshedVisibleChanged();
//}

//QStringList AnalysisController::viewshedTypes() const
//{
//  return m_viewshedTypes;
//}

//int AnalysisController::viewshedTypeIndex() const
//{
//  return m_viewshedTypeIndex;
//}

//void AnalysisController::setViewshedTypeIndex(int index)
//{
//  if (m_viewshedTypeIndex == index)
//    return;

//  m_viewshedTypeIndex = index;

//  updateCurrentViewshed();

//  emit viewshedTypeIndexChanged();
//}

//double AnalysisController::minDistance() const
//{
//  return m_currentViewshed ? m_currentViewshed->minDistance() : NAN;
//}

//void AnalysisController::setMinDistance(double minDistance)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->minDistance() == minDistance)
//    return;

//  m_currentViewshed->setMinDistance(minDistance);

//  emit minDistanceChanged();
//}

//double AnalysisController::maxDistance() const
//{
//  return m_currentViewshed ? m_currentViewshed->maxDistance() : NAN;
//}

//void AnalysisController::setMaxDistance(double maxDistance)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->maxDistance() == maxDistance)
//    return;

//  m_currentViewshed->setMaxDistance(maxDistance);

//  emit maxDistanceChanged();
//}

//double AnalysisController::horizontalAngle() const
//{
//  return m_currentViewshed ? m_currentViewshed->horizontalAngle() : NAN;
//}

//void AnalysisController::setHorizontalAngle(double horizontalAngle)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->horizontalAngle() == horizontalAngle)
//    return;

//  m_currentViewshed->setHorizontalAngle(horizontalAngle);

//  emit horizontalAngleChanged();
//}

//double AnalysisController::verticalAngle() const
//{
//  return m_currentViewshed ? m_currentViewshed->verticalAngle() : NAN;
//}

//void AnalysisController::setVerticalAngle(double verticalAngle)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->verticalAngle() == verticalAngle)
//    return;

//  m_currentViewshed->setVerticalAngle(verticalAngle);

//  emit verticalAngleChanged();
//}

//double AnalysisController::heading() const
//{
//  return m_currentViewshed ? m_currentViewshed->heading() : NAN;
//}

//void AnalysisController::setHeading(double heading)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->heading() == heading)
//    return;

//  m_currentViewshed->setHeading(heading);

//  emit headingChanged();
//}

//double AnalysisController::pitch() const
//{
//  return m_currentViewshed ? m_currentViewshed->pitch() : NAN;
//}

//void AnalysisController::setPitch(double pitch)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->pitch() == pitch)
//    return;

//  m_currentViewshed->setPitch(pitch);

//  emit pitchChanged();
//}

//bool AnalysisController::isViewshed360Override() const
//{
//  return m_currentViewshed ? m_currentViewshed->is360Mode() : false;
//}

//void AnalysisController::setViewshed360Override(bool viewshed360Override)
//{
//  if (!m_viewshedEnabled || !m_currentViewshed)
//    return;

//  if (m_currentViewshed->is360Mode() == viewshed360Override)
//    return;

//  m_currentViewshed->set360Mode(viewshed360Override);

//  emit viewshed360OverrideChanged();
//  emit horizontalAngleChanged();
//  emit verticalAngleChanged();
//}

//void AnalysisController::emitAllChanged()
//{
//  emit viewshedVisibleChanged();
//  emit minDistanceChanged();
//  emit maxDistanceChanged();
//  emit horizontalAngleChanged();
//  emit verticalAngleChanged();
//  emit headingChanged();
//  emit pitchChanged();
//  emit viewshed360OverrideChanged();
//}

QString AnalysisController::toolName() const
{
  return QStringLiteral("analysis");
}
