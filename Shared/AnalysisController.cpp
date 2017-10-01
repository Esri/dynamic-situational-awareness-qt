// Copyright 2016 ESRI
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
#include "SceneQuickView.h"
#include "LocationViewshed.h"

using namespace Esri::ArcGISRuntime;

AnalysisController::AnalysisController(QObject *parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance()->addTool(this);

  Camera camera(Point(-121.9, 36.6, 1000, SpatialReference::wgs84()), 1000, 90, 90, 0);
  m_locationViewshed = new LocationViewshed(camera, 100, 1000, this);
}

AnalysisController::~AnalysisController()
{
}

void AnalysisController::init(GeoView* geoView)
{
  //TODO:mt hard-coded for now
  m_sceneView = static_cast<SceneQuickView*>(geoView);
  connect(m_sceneView, &SceneQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    if (!m_analysisVisible)
      return;

    if (m_viewshedActive)
    {
      m_viewshedActive = false;
      return;
    }

    if (m_sceneView->analysisOverlays()->size() == 0)
    {
      AnalysisOverlay* analysisOverlay = new AnalysisOverlay(this);
      m_sceneView->analysisOverlays()->append(analysisOverlay);

      const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
      Point elevatedPt(pt.x(), pt.y(), pt.z() + 50, pt.spatialReference());
      m_locationViewshed->setLocation(elevatedPt);

      analysisOverlay->analyses()->append(m_locationViewshed);
      return;
    }

    const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
    Point elevatedPt(pt.x(), pt.y(), pt.z() + 50, pt.spatialReference());
    m_locationViewshed->setLocation(elevatedPt);
  });

  connect(m_sceneView, &SceneQuickView::mousePressed, this, [this](QMouseEvent& event)
  {
    if (!m_analysisVisible)
      return;

    m_currentMouseButton = event.button();
  });

  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld, this, [this](QMouseEvent& event)
  {
    if (!m_analysisVisible)
      return;

    m_viewshedActive = true;

    m_pressedPoint = event.pos();
  });

  connect(m_sceneView, &SceneQuickView::mouseMoved, this, [this](QMouseEvent& event)
  {
    if (!m_analysisVisible || !m_viewshedActive)
      return;


    if (m_currentMouseButton == Qt::LeftButton)
    {
      const Point pt = m_sceneView->screenToBaseSurface(event.x(), event.y());
      Point elevatedPt(pt.x(), pt.y(), pt.z() + 50, pt.spatialReference());
      m_locationViewshed->setLocation(elevatedPt);
    }
    else if (m_currentMouseButton == Qt::RightButton)
    {
      const QPoint curPoint = event.pos();
      if (m_pressedPoint.x() < curPoint.x())
        m_locationViewshed->setHeading(m_locationViewshed->heading() + 3);
      else
        m_locationViewshed->setHeading(m_locationViewshed->heading() - 3);

      m_pressedPoint = curPoint;
    }
  });
}

bool AnalysisController::isAnalysisVisible() const
{
  return m_analysisVisible;
}

void AnalysisController::setAnalysisVisible(bool analysisVisible)
{
  if (m_analysisVisible == analysisVisible)
    return;

  m_analysisVisible = analysisVisible;

  emit analysisVisibleChanged();
}

double AnalysisController::minDistance() const
{
  return m_locationViewshed->minDistance();
}

void AnalysisController::setMinDistance(double minDistance)
{
  if (m_locationViewshed->minDistance() == minDistance)
    return;

  m_locationViewshed->setMinDistance(minDistance);

  emit minDistanceChanged();
}

double AnalysisController::maxDistance() const
{
  return m_locationViewshed->maxDistance();
}

void AnalysisController::setMaxDistance(double maxDistance)
{
  if (m_locationViewshed->maxDistance() == maxDistance)
    return;

  m_locationViewshed->setMaxDistance(maxDistance);

  emit maxDistanceChanged();
}

double AnalysisController::horizontalAngle() const
{
  return m_locationViewshed->horizontalAngle();
}

void AnalysisController::setHorizontalAngle(double horizontalAngle)
{
  if (m_locationViewshed->horizontalAngle() == horizontalAngle)
    return;

  m_locationViewshed->setHorizontalAngle(horizontalAngle);

  emit horizontalAngleChanged();
}

double AnalysisController::verticalAngle() const
{
  return m_locationViewshed->verticalAngle();
}

void AnalysisController::setVerticalAngle(double verticalAngle)
{
  if (m_locationViewshed->verticalAngle() == verticalAngle)
    return;

  m_locationViewshed->setVerticalAngle(verticalAngle);

  emit verticalAngleChanged();
}

double AnalysisController::heading() const
{
  return m_locationViewshed->heading();
}

void AnalysisController::setHeading(double heading)
{
  if (m_locationViewshed->heading() == heading)
    return;

  m_locationViewshed->setHeading(heading);

  emit headingChanged();
}

QString AnalysisController::toolName() const
{
  return QStringLiteral("analysis");
}
