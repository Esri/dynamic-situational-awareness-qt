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
#include "SimpleMarkerSceneSymbol.h"

using namespace Esri::ArcGISRuntime;

const QString AnalysisController::MAP_POINT_VIEWSHED_TYPE = QStringLiteral("Map Point");
const QString AnalysisController::MY_LOCATION_VIEWSHED_TYPE = QStringLiteral("My Location");
const QString AnalysisController::FRIENDLY_TRACK_VIEWSHED_TYPE = QStringLiteral("Friendly Track");

AnalysisController::AnalysisController(QObject *parent) :
  Toolkit::AbstractTool(parent),
  m_analysisOverlay(new AnalysisOverlay(this)),
  m_viewsheds({nullptr, nullptr, nullptr}),
  m_viewshedTypes({MAP_POINT_VIEWSHED_TYPE,
                  MY_LOCATION_VIEWSHED_TYPE,
                  FRIENDLY_TRACK_VIEWSHED_TYPE})
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]
  {
    updateGeoView();
  });

  connectMouseSignals();

  updateGeoView();
}

AnalysisController::~AnalysisController()
{
}

void AnalysisController::updateGeoView()
{
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  sceneView->analysisOverlays()->append(m_analysisOverlay);
}

void AnalysisController::connectMouseSignals()
{
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked, this, [this](QMouseEvent& event)
  {
    if (!m_viewshedEnabled)
      return;

    SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
    if (!sceneView)
      return;

    if (!m_analysisOverlay && !m_graphicsOverlay)
    {
      m_analysisOverlay = new AnalysisOverlay(this);
      sceneView->analysisOverlays()->append(m_analysisOverlay);

      m_graphicsOverlay = new GraphicsOverlay(this);
      m_graphicsOverlay->setOverlayId("Analysis overlay");
      sceneView->graphicsOverlays()->append(m_graphicsOverlay);

      return;
    }

    if (!m_locationViewshed && !m_locationViewshedGraphic)
    {
      const Point pt = sceneView->screenToBaseSurface(event.x(), event.y());

      m_locationViewshed = new LocationViewshed(pt, m_headingDefault, m_pitchDefault, m_horizontalAngleDefault, m_verticalAngleDefault, m_minDistanceDefault, m_maxDistanceDefault, this);
      m_locationViewshed->setVisible(m_viewshedVisibleDefault);
      m_analysisOverlay->analyses()->append(m_locationViewshed);

      SimpleMarkerSceneSymbol* smss = SimpleMarkerSceneSymbol::cone(QColor("red"), 0.5, 70, this);
      smss->setWidth(12);
      m_locationViewshedGraphic = new Graphic(m_locationViewshed->location(), smss, this);
      m_graphicsOverlay->graphics()->append(m_locationViewshedGraphic);

      smss->setHeading(m_locationViewshed->heading()-180);
      smss->setPitch(qAbs(m_locationViewshed->pitch()-180));

      return;
    }

    const Point pt = sceneView->screenToBaseSurface(event.x(), event.y());
    m_locationViewshed->setLocation(pt);
    m_locationViewshedGraphic->setGeometry(pt);
  });
}

void AnalysisController::removeViewshed()
{
  if (m_locationViewshed && m_locationViewshedGraphic)
  {
    m_analysisOverlay->analyses()->removeOne(m_locationViewshed);
    delete m_locationViewshed;
    m_locationViewshed = nullptr;

    m_graphicsOverlay->graphics()->removeOne(m_locationViewshedGraphic);
    delete m_locationViewshedGraphic;
    m_locationViewshedGraphic = nullptr;
  }
}

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

bool AnalysisController::isViewshedVisible() const
{
  return m_locationViewshed ? m_locationViewshed->isVisible() : m_viewshedVisibleDefault;
}

void AnalysisController::setViewshedVisible(bool viewshedVisible)
{
  if (!m_viewshedEnabled)
    return;

  if (m_locationViewshed && m_locationViewshedGraphic)
  {
    if (m_locationViewshed->isVisible() == viewshedVisible)
      return;

    m_viewshedVisibleDefault = viewshedVisible;
    m_locationViewshed->setVisible(viewshedVisible);
    m_locationViewshedGraphic->setVisible(viewshedVisible);
  }
  else
  {
    if (m_viewshedVisibleDefault == viewshedVisible)
      return;

    m_viewshedVisibleDefault = viewshedVisible;
  }

  emit viewshedVisibleChanged();
}

QStringList AnalysisController::viewshedTypes() const
{
  return m_viewshedTypes;
}

int AnalysisController::viewshedTypeIndex() const
{
  return m_viewshedTypeIndex;
}

void AnalysisController::setViewshedTypeIndex(int index)
{
  if (m_viewshedTypeIndex == index)
    return;

  m_viewshedTypeIndex = index;

  emit viewshedTypeIndexChanged();
}

double AnalysisController::minDistance() const
{
  return m_locationViewshed ? m_locationViewshed->minDistance() : m_minDistanceDefault;
}

void AnalysisController::setMinDistance(double minDistance)
{
  if (!m_viewshedEnabled)
    return;

  if (m_locationViewshed)
  {
    if (m_locationViewshed->minDistance() == minDistance)
      return;

    m_minDistanceDefault = minDistance;
    m_locationViewshed->setMinDistance(minDistance);
  }
  else
  {
    if (m_minDistanceDefault == minDistance)
      return;

    m_minDistanceDefault = minDistance;
  }

  emit minDistanceChanged();
}

double AnalysisController::maxDistance() const
{
  return m_locationViewshed ? m_locationViewshed->maxDistance() : m_maxDistanceDefault;
}

void AnalysisController::setMaxDistance(double maxDistance)
{
  if (!m_viewshedEnabled)
    return;

  if (m_locationViewshed)
  {
    if (m_locationViewshed->maxDistance() == maxDistance)
      return;

    m_maxDistanceDefault = maxDistance;
    m_locationViewshed->setMaxDistance(maxDistance);
  }
  else
  {
    if (m_maxDistanceDefault == maxDistance)
      return;

    m_maxDistanceDefault = maxDistance;
  }

  emit maxDistanceChanged();
}

double AnalysisController::horizontalAngle() const
{
  return m_locationViewshed ? m_locationViewshed->horizontalAngle() : m_horizontalAngleDefault;
}

void AnalysisController::setHorizontalAngle(double horizontalAngle)
{
  if (!m_viewshedEnabled)
    return;

  if (m_locationViewshed)
  {
    if (m_locationViewshed->horizontalAngle() == horizontalAngle)
      return;

    m_horizontalAngleDefault = horizontalAngle;
    m_locationViewshed->setHorizontalAngle(horizontalAngle);
  }
  else
  {
    if (m_horizontalAngleDefault == horizontalAngle)
      return;

    m_horizontalAngleDefault = horizontalAngle;
  }

  emit horizontalAngleChanged();
}

double AnalysisController::verticalAngle() const
{
  return m_locationViewshed ? m_locationViewshed->verticalAngle() : m_verticalAngleDefault;
}

void AnalysisController::setVerticalAngle(double verticalAngle)
{
  if (!m_viewshedEnabled)
    return;

  if (m_locationViewshed)
  {
    if (m_locationViewshed->verticalAngle() == verticalAngle)
      return;

    m_verticalAngleDefault = verticalAngle;
    m_locationViewshed->setVerticalAngle(verticalAngle);
  }
  else
  {
    if (m_verticalAngleDefault == verticalAngle)
      return;

    m_verticalAngleDefault = verticalAngle;
  }

  emit verticalAngleChanged();
}

double AnalysisController::heading() const
{
  return m_locationViewshed ? m_locationViewshed->heading() : m_headingDefault;
}

void AnalysisController::setHeading(double heading)
{
  if (!m_viewshedEnabled)
    return;

  if (m_locationViewshed && m_locationViewshedGraphic)
  {
    if (m_locationViewshed->heading() == heading)
      return;

    m_headingDefault = heading;
    m_locationViewshed->setHeading(heading);

    SimpleMarkerSceneSymbol* smss = static_cast<SimpleMarkerSceneSymbol*>(m_locationViewshedGraphic->symbol());
    smss->setHeading(m_locationViewshed->heading()-180);
  }
  else
  {
    if (m_headingDefault == heading)
      return;

    m_headingDefault = heading;
  }

  emit headingChanged();
}

double AnalysisController::pitch() const
{
  return m_locationViewshed ? m_locationViewshed->pitch() : m_pitchDefault;
}

void AnalysisController::setPitch(double pitch)
{
  if (!m_viewshedEnabled)
    return;

  if (m_locationViewshed)
  {
    if (m_locationViewshed->pitch() == pitch)
      return;

    m_pitchDefault = pitch;
    m_locationViewshed->setPitch(pitch);

    SimpleMarkerSceneSymbol* smss = static_cast<SimpleMarkerSceneSymbol*>(m_locationViewshedGraphic->symbol());
    smss->setPitch(qAbs(m_locationViewshed->pitch()-180));
  }
  else
  {
    if (m_pitchDefault == pitch)
      return;

    m_pitchDefault = pitch;
  }

  emit pitchChanged();
}

QString AnalysisController::toolName() const
{
  return QStringLiteral("analysis");
}
