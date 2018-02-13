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
#include "GeoElementViewshed.h"
#include "LocationController.h"
#include "LocationDisplay3d.h"
#include "IdentifyController.h"

using namespace Esri::ArcGISRuntime;

const QString AnalysisController::MAP_POINT_VIEWSHED_TYPE = QStringLiteral("Map Point");
const QString AnalysisController::MY_LOCATION_VIEWSHED_TYPE = QStringLiteral("My Location");
const QString AnalysisController::FRIENDLY_TRACK_VIEWSHED_TYPE = QStringLiteral("Friendly Track");

static const QString s_headingAttribute{ QStringLiteral("heading") };
static const QString s_pitchAttribute{ QStringLiteral("pitch") };

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

    if (m_viewshedTypeIndex == 0)
    {
      updateMapPointViewshed(event);
      return;
    }

    if (m_viewshedTypeIndex == 2)
    {
      updateFriendlyTrackViewshed(event);
      return;
    }
  });
}

void AnalysisController::updateCurrentViewshed()
{
  auto viewshed = m_viewsheds[m_viewshedTypeIndex];
  if (m_currentViewshed && m_currentViewshed == viewshed)
    return;

  m_currentViewshed = viewshed;

  if (m_viewshedTypeIndex == 1)
    updateMyLocationViewshed();

  emitAllChanged();
}

void AnalysisController::updateMapPointViewshed(QMouseEvent& event)
{
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  if (!m_graphicsOverlay)
  {
    m_graphicsOverlay = new GraphicsOverlay(this);
    m_graphicsOverlay->setOverlayId("Analysis overlay");
    sceneView->graphicsOverlays()->append(m_graphicsOverlay);
  }

  if (!m_currentViewshed && !m_locationViewshedGraphic)
  {
    const Point pt = sceneView->screenToBaseSurface(event.x(), event.y());

    auto locationViewshed = new LocationViewshed(pt, m_headingDefault, m_pitchDefault,
                                                 m_horizontalAngleDefault, m_verticalAngleDefault,
                                                 m_minDistanceDefault, m_maxDistanceDefault, this);
    m_currentViewshed = locationViewshed;
    m_viewsheds[m_viewshedTypeIndex] = locationViewshed;
    locationViewshed->setVisible(m_viewshedVisibleDefault);
    m_analysisOverlay->analyses()->append(locationViewshed);

    SimpleMarkerSceneSymbol* smss = SimpleMarkerSceneSymbol::cone(QColor("red"), 0.5, 70, this);
    smss->setWidth(12);
    m_locationViewshedGraphic = new Graphic(locationViewshed->location(), smss, this);
    m_graphicsOverlay->graphics()->append(m_locationViewshedGraphic);

    smss->setHeading(locationViewshed->heading()-180);
    smss->setPitch(qAbs(locationViewshed->pitch()-180));
  }

  if (!m_currentViewshed)
    return;

  const Point pt = sceneView->screenToBaseSurface(event.x(), event.y());
  static_cast<LocationViewshed*>(m_currentViewshed)->setLocation(pt);
  m_locationViewshedGraphic->setGeometry(pt);
}

void AnalysisController::updateMyLocationViewshed()
{
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  if (!m_currentViewshed)
  {
    LocationController* locationController = Toolkit::ToolManager::instance().tool<LocationController>();
    if (locationController)
    {
      Graphic* locationGraphic = locationController->locationDisplay()->locationGraphic();
      auto geoElementViewshed = new GeoElementViewshed(locationGraphic, m_horizontalAngleDefault, m_verticalAngleDefault,
                                                       m_minDistanceDefault, m_maxDistanceDefault, 0.0, 0.0, this);

      geoElementViewshed->setOffsetZ(5.0);
      m_currentViewshed = geoElementViewshed;
      m_viewsheds[m_viewshedTypeIndex] = geoElementViewshed;
      geoElementViewshed->setVisible(m_viewshedVisibleDefault);
      m_analysisOverlay->analyses()->append(geoElementViewshed);
    }
  }
}

void AnalysisController::updateFriendlyTrackViewshed(QMouseEvent& event)
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

      if (m_currentViewshed)
      {
        GeoElement* geoElement = static_cast<GeoElementViewshed*>(m_currentViewshed)->geoElement();
        if (qobject_cast<Graphic*>(geoElement) == graphic)
        {
          qDeleteAll(identifyResults); // TODO: should use the GraphicsOverlaysResultsManager from the IdentifyController instead
          return;
        }

        removeViewshed();
      }

      auto geoElementViewshed = new GeoElementViewshed(graphic, m_horizontalAngleDefault, m_verticalAngleDefault,
                                                       m_minDistanceDefault, m_maxDistanceDefault,
                                                       m_headingOffsetDefault, m_pitchOffsetDefault, this);

      geoElementViewshed->setOffsetZ(5.0);
      m_currentViewshed = geoElementViewshed;
      m_viewsheds[m_viewshedTypeIndex] = geoElementViewshed;
      geoElementViewshed->setVisible(m_viewshedVisibleDefault);
      m_analysisOverlay->analyses()->append(geoElementViewshed);

      emit headingChanged();
      emit pitchChanged();

      qDeleteAll(identifyResults); // TODO: should use the GraphicsOverlaysResultsManager from the IdentifyController instead
    });
  }

  m_identifyTaskWatcher = Toolkit::ToolResourceProvider::instance()->geoView()->identifyGraphicsOverlays(event.x(), event.y(), 5.0, false, 1);
}

void AnalysisController::createViewsheds360Offsets()
{
  if (!m_currentViewshed || m_viewshedTypeIndex != 1)
    return;

  GeoElement* geoElement = static_cast<GeoElementViewshed*>(m_currentViewshed)->geoElement();

  auto geoElementViewshedOffset1 = new GeoElementViewshed(geoElement, 120.0, m_verticalAngleDefault,
                                                          m_minDistanceDefault, m_maxDistanceDefault, 120.0, 0.0, this);

  geoElementViewshedOffset1->setOffsetZ(5.0);
  geoElementViewshedOffset1->setVisible(m_viewshedVisibleDefault);
  m_analysisOverlay->analyses()->append(geoElementViewshedOffset1);
  m_viewsheds360Offsets.append(geoElementViewshedOffset1);

  auto geoElementViewshedOffset2 = new GeoElementViewshed(geoElement, 120.0, m_verticalAngleDefault,
                                                          m_minDistanceDefault, m_maxDistanceDefault, 240.0, 0.0, this);

  geoElementViewshedOffset2->setOffsetZ(5.0);
  geoElementViewshedOffset2->setVisible(m_viewshedVisibleDefault);
  m_analysisOverlay->analyses()->append(geoElementViewshedOffset2);
  m_viewsheds360Offsets.append(geoElementViewshedOffset2);
}

void AnalysisController::removeViewshed()
{
  if (!m_currentViewshed)
    return;

  m_analysisOverlay->analyses()->removeOne(m_currentViewshed);

  delete m_currentViewshed;
  m_currentViewshed = nullptr;
  m_viewsheds[m_viewshedTypeIndex] = nullptr;

  if (m_viewshedTypeIndex == 0 && m_locationViewshedGraphic)
  {
    m_graphicsOverlay->graphics()->removeOne(m_locationViewshedGraphic);
    delete m_locationViewshedGraphic;
    m_locationViewshedGraphic = nullptr;
  }
  else if (m_viewshedTypeIndex == 1)
  {
    for (auto viewshed : m_viewsheds360Offsets)
    {
      m_analysisOverlay->analyses()->removeOne(viewshed);
      delete viewshed;
    }

    m_viewsheds360Offsets.clear();
    m_viewshed360Override = false;
  }

  emitAllChanged();
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
  return m_currentViewshed ? m_currentViewshed->isVisible() : m_viewshedVisibleDefault;
}

void AnalysisController::setViewshedVisible(bool viewshedVisible)
{
  if (!m_viewshedEnabled)
    return;

  if (m_currentViewshed)
  {
    if (m_currentViewshed->isVisible() == viewshedVisible)
      return;

    m_viewshedVisibleDefault = viewshedVisible;
    m_currentViewshed->setVisible(viewshedVisible);

    if (m_viewshedTypeIndex == 0 && m_locationViewshedGraphic)
      m_locationViewshedGraphic->setVisible(viewshedVisible);

    if (m_viewshedTypeIndex == 1)
    {
      for (auto viewshed : m_viewsheds360Offsets)
      {
        viewshed->setVisible(viewshedVisible && m_viewshed360Override);
      }
    }
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

  updateCurrentViewshed();

  emit viewshedTypeIndexChanged();
}

double AnalysisController::minDistance() const
{
  return m_currentViewshed ? m_currentViewshed->minDistance() : m_minDistanceDefault;
}

void AnalysisController::setMinDistance(double minDistance)
{
  if (!m_viewshedEnabled)
    return;

  if (m_currentViewshed)
  {
    if (m_currentViewshed->minDistance() == minDistance)
      return;

    m_minDistanceDefault = minDistance;
    m_currentViewshed->setMinDistance(minDistance);

    if (m_viewshedTypeIndex == 1)
    {
      for (auto viewshed : m_viewsheds360Offsets)
      {
        viewshed->setMinDistance(minDistance);
      }
    }
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
  return m_currentViewshed ? m_currentViewshed->maxDistance() : m_maxDistanceDefault;
}

void AnalysisController::setMaxDistance(double maxDistance)
{
  if (!m_viewshedEnabled)
    return;

  if (m_currentViewshed)
  {
    if (m_currentViewshed->maxDistance() == maxDistance)
      return;

    m_maxDistanceDefault = maxDistance;
    m_currentViewshed->setMaxDistance(maxDistance);

    if (m_viewshedTypeIndex == 1)
    {
      for (auto viewshed : m_viewsheds360Offsets)
      {
        viewshed->setMaxDistance(maxDistance);
      }
    }
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
  if (m_viewshed360Override && m_viewshedTypeIndex == 1)
    return 360.0; // 360 viewshed override

  return m_currentViewshed ? m_currentViewshed->horizontalAngle() : m_horizontalAngleDefault;
}

void AnalysisController::setHorizontalAngle(double horizontalAngle)
{
  if (!m_viewshedEnabled)
    return;

  if (m_currentViewshed)
  {
    if (m_currentViewshed->horizontalAngle() == horizontalAngle)
      return;

    m_horizontalAngleDefault = horizontalAngle;
    m_currentViewshed->setHorizontalAngle(horizontalAngle);
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
  if (m_viewshed360Override && m_viewshedTypeIndex == 1)
    return 90.0; // 360 viewshed override

  return m_currentViewshed ? m_currentViewshed->verticalAngle() : m_verticalAngleDefault;
}

void AnalysisController::setVerticalAngle(double verticalAngle)
{
  if (!m_viewshedEnabled)
    return;

  if (m_currentViewshed)
  {
    if (m_currentViewshed->verticalAngle() == verticalAngle)
      return;

    m_verticalAngleDefault = verticalAngle;
    m_currentViewshed->setVerticalAngle(verticalAngle);
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
  if (!m_currentViewshed)
    return m_headingDefault;

  if (m_viewshedTypeIndex == 0)
    return static_cast<LocationViewshed*>(m_currentViewshed)->heading();

  if (m_viewshedTypeIndex == 2)
    return static_cast<GeoElementViewshed*>(m_currentViewshed)->headingOffset();

  return static_cast<GeoElementViewshed*>(m_currentViewshed)->geoElement()->attributes()->attributeValue(s_headingAttribute).toDouble();
}

void AnalysisController::setHeading(double heading)
{
  if (!m_viewshedEnabled)
    return;

  if (!m_currentViewshed)
  {
    if (m_headingDefault == heading)
      return;

    m_headingDefault = heading;
  }
  else if (m_viewshedTypeIndex == 0)
  {
    if (static_cast<LocationViewshed*>(m_currentViewshed)->heading() == heading)
      return;

    m_headingDefault = heading;
    static_cast<LocationViewshed*>(m_currentViewshed)->setHeading(heading);

    if (m_locationViewshedGraphic)
    {
      SimpleMarkerSceneSymbol* smss = static_cast<SimpleMarkerSceneSymbol*>(m_locationViewshedGraphic->symbol());
      smss->setHeading(static_cast<LocationViewshed*>(m_currentViewshed)->heading()-180);
    }
  }
  else if (m_viewshedTypeIndex == 2)
  {
    if (static_cast<GeoElementViewshed*>(m_currentViewshed)->headingOffset() == heading)
      return;

    m_headingOffsetDefault = heading;
    static_cast<GeoElementViewshed*>(m_currentViewshed)->setHeadingOffset(heading);
  }
  else
  {
    auto attribs = static_cast<GeoElementViewshed*>(m_currentViewshed)->geoElement()->attributes();
    if (attribs->attributeValue(s_headingAttribute).toDouble() == heading)
      return;

    attribs->replaceAttribute(s_headingAttribute, heading);
  }

  emit headingChanged();
}

double AnalysisController::pitch() const
{
  if (!m_currentViewshed)
    return m_pitchDefault;

  if (m_viewshedTypeIndex == 0)
    return static_cast<LocationViewshed*>(m_currentViewshed)->pitch();

  if (m_viewshedTypeIndex == 2)
    return static_cast<GeoElementViewshed*>(m_currentViewshed)->pitchOffset();

  return static_cast<GeoElementViewshed*>(m_currentViewshed)->geoElement()->attributes()->attributeValue(s_pitchAttribute).toDouble();
}

void AnalysisController::setPitch(double pitch)
{
  if (!m_viewshedEnabled)
    return;

  if (!m_currentViewshed)
  {
    if (m_pitchDefault == pitch)
      return;

    m_pitchDefault = pitch;
  }
  else if (m_viewshedTypeIndex == 0)
  {
    if (static_cast<LocationViewshed*>(m_currentViewshed)->pitch() == pitch)
      return;

    m_pitchDefault = pitch;
    static_cast<LocationViewshed*>(m_currentViewshed)->setPitch(pitch);

    if (m_locationViewshedGraphic)
    {
      SimpleMarkerSceneSymbol* smss = static_cast<SimpleMarkerSceneSymbol*>(m_locationViewshedGraphic->symbol());
      smss->setPitch(static_cast<LocationViewshed*>(m_currentViewshed)->pitch());
    }
  }
  else if (m_viewshedTypeIndex == 2)
  {
    if (static_cast<GeoElementViewshed*>(m_currentViewshed)->pitchOffset() == pitch)
      return;

    m_pitchOffsetDefault = pitch;
    static_cast<GeoElementViewshed*>(m_currentViewshed)->setPitchOffset(pitch);
  }
  else
  {
    auto attribs = static_cast<GeoElementViewshed*>(m_currentViewshed)->geoElement()->attributes();
    if (attribs->attributeValue(s_pitchAttribute).toDouble() == pitch)
      return;

    attribs->replaceAttribute(s_pitchAttribute, pitch);
  }

  emit pitchChanged();
}

bool AnalysisController::isViewshed360Override() const
{
  return m_viewshed360Override;
}

void AnalysisController::setViewshed360Override(bool viewshed360Override)
{
  if (!m_currentViewshed || m_viewshedTypeIndex != 1 ||
      m_viewshed360Override == viewshed360Override)
    return;

  m_viewshed360Override = viewshed360Override;

  emit viewshed360OverrideChanged();

  if (m_viewshed360Override && m_viewsheds360Offsets.isEmpty())
    createViewsheds360Offsets();

  m_currentViewshed->setHorizontalAngle(120.0);
  m_currentViewshed->setVerticalAngle(90.0);

  emit horizontalAngleChanged();
  emit verticalAngleChanged();

  for (auto viewshed : m_viewsheds360Offsets)
  {
    viewshed->setVisible(m_currentViewshed->isVisible() && viewshed360Override);
  }
}

void AnalysisController::emitAllChanged()
{
  emit viewshedVisibleChanged();
  emit minDistanceChanged();
  emit maxDistanceChanged();
  emit horizontalAngleChanged();
  emit verticalAngleChanged();
  emit headingChanged();
  emit pitchChanged();
  emit viewshed360OverrideChanged();
}

QString AnalysisController::toolName() const
{
  return QStringLiteral("analysis");
}
