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

#include "LocationController.h"

#include <QCompass>
#include <QDir>
#include <cmath>

#include "SceneQuickView.h"
#include "Point.h"
#include "GraphicsOverlay.h"
#include "ModelSceneSymbol.h"
#include "DistanceCompositeSceneSymbol.h"
#include "SimpleRenderer.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "GPXLocationSimulator.h"

using namespace Esri::ArcGISRuntime;

const QString LocationController::SIMULATE_LOCATION_PROPERTYNAME = "SimulateLocation";
const QString LocationController::GPX_FILE_PROPERTYNAME = "GpxFile";
const QString LocationController::RESOURCE_DIRECTORY_PROPERTYNAME = "ResourceDirectory";

LocationController::LocationController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::geoViewChanged, this, &LocationController::updateGeoView);

  updateGeoView();
}

LocationController::~LocationController()
{
}

void LocationController::initPositionInfoSource(bool simulated)
{
  if (simulated && !m_simulator)
  {
    m_simulator = new GPXLocationSimulator(this);

    connect(m_simulator, &GPXLocationSimulator::positionUpdateAvailable, this,
    [this](const Point& pos, double heading)
    {
      m_lastKnownHeading = heading;

      emit positionChanged(pos);
      emit headingChanged(heading);
      emit relativeHeadingChanged(heading - m_lastViewHeading);
    });
  }
  else if (!simulated && !m_positionSource)
  {
    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);

    connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated, this,
    [this](const QGeoPositionInfo& update)
    {
      if (!update.isValid())
        return;

      const auto pos = update.coordinate();

      if (!pos.isValid())
        return;

      Point newPosition;
      switch (pos.type())
      {
        case QGeoCoordinate::Coordinate2D:
          newPosition = Point(pos.longitude(), pos.latitude(), SpatialReference::wgs84());
          break;
        case QGeoCoordinate::Coordinate3D:
          newPosition = Point(pos.longitude(), pos.latitude(), pos.altitude(), SpatialReference::wgs84());
          break;
        case QGeoCoordinate::InvalidCoordinate:
        default:
          return;
      }

      emit positionChanged(newPosition);
    });

    m_compass = new QCompass(this);

    connect(m_compass, &QCompass::readingChanged, this, [this]()
    {
      QCompassReading* reading = m_compass->reading();
      if (!reading)
        return;

      emit headingChanged(static_cast<double>(reading->azimuth()));
    });
  }
}

QString LocationController::toolName() const
{
  return QStringLiteral("location");
}

void LocationController::setProperties(const QVariantMap& properties)
{
  bool simulate = QString::compare(properties[SIMULATE_LOCATION_PROPERTYNAME].toString(), QString("true"), Qt::CaseInsensitive) == 0;
  setSimulated(simulate);
  setGpxFilePath(QUrl::fromLocalFile(properties[GPX_FILE_PROPERTYNAME].toString()));
  setIconDataPath(properties[RESOURCE_DIRECTORY_PROPERTYNAME].toString());
}

bool LocationController::isEnabled() const
{
  return m_enabled;
}

void LocationController::setEnabled(bool enabled)
{
  if (m_enabled == enabled)
    return;

  initPositionInfoSource(m_simulated);

  if (m_locationOverlay)
    m_locationOverlay->setVisible(enabled);

  if (m_simulated)
  {
    if (enabled)
      m_simulator->startUpdates();
    else
      m_simulator->stopUpdates();
  }
  else
  {
    if (enabled)
    {
      m_positionSource->startUpdates();
      m_compass->start();
    }
    else
    {
      m_positionSource->stopUpdates();
      m_compass->stop();
    }
  }

  m_enabled = enabled;
  emit enabledChanged();
}

bool LocationController::isSimulated() const
{
  return m_simulated;
}

void LocationController::setSimulated(bool simulated)
{
  if (m_simulated == simulated)
    return;

  if (simulated)
    initPositionInfoSource(simulated);

  m_simulated = simulated;
  emit simulatedChanged();
  emit propertyUpdated(SIMULATE_LOCATION_PROPERTYNAME, m_simulated);
}

QUrl LocationController::gpxFilePath() const
{
  return m_gpxFilePath;
}

QString LocationController::gpxFilePathAsString() const
{
  return m_gpxFilePath.toLocalFile();
}

void LocationController::setGpxFilePath(const QUrl& gpxFilePath)
{
  if (m_gpxFilePath == gpxFilePath)
    return;

  initPositionInfoSource(true); // ignore m_simulated, we need to init the simulator now

  m_simulator->setGpxFile(gpxFilePath.toLocalFile());

  m_gpxFilePath = gpxFilePath;
  emit gpxFilePathChanged();
  emit propertyUpdated(GPX_FILE_PROPERTYNAME, m_gpxFilePath);
}

void LocationController::setRelativeHeadingSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView)
{
  connect(sceneView, &SceneQuickView::viewpointChanged, this,
  [sceneView, this]()
  {
    const auto sceneViewHeading = sceneView->currentViewpointCamera().heading();
    if (std::abs(m_lastViewHeading - sceneViewHeading) < 0.1)
      return;

    m_lastViewHeading = sceneViewHeading;

    // keep the orientation correct if we're not doing any updates
    if (!m_enabled)
      emit relativeHeadingChanged(m_lastKnownHeading + m_lastViewHeading);
  });
}

void LocationController::updateGeoView()
{
  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (geoView)
    geoView->graphicsOverlays()->append(locationOverlay());
}

GraphicsOverlay* LocationController::locationOverlay()
{
  if (!m_locationOverlay)
    initOverlay();

  return m_locationOverlay;
}

void LocationController::initOverlay()
{
  constexpr float symbolSize = 45.0;
  constexpr double rangeMultiplier = 1.04; // the closer to 1.0, the smoother the transitions
  constexpr double maxRange = 10000000.0;

  const QUrl modelPath = modelSymbolPath();

  ModelSceneSymbol* modelSceneSymbol = new ModelSceneSymbol(modelPath, this);
  modelSceneSymbol->setWidth(symbolSize);
  modelSceneSymbol->setDepth(symbolSize);

  DistanceCompositeSceneSymbol* distanceCompSymbol = new DistanceCompositeSceneSymbol(this);
  distanceCompSymbol->ranges()->append(new DistanceSymbolRange(modelSceneSymbol, 0.0, 1000.0, this));

  float rangeSize = symbolSize;
  for (double i = 1000.0; i < maxRange; i *= rangeMultiplier)
  {
    ModelSceneSymbol* rangeSym = new ModelSceneSymbol(modelPath, this);
    rangeSize *= static_cast<float>(rangeMultiplier);
    rangeSym->setWidth(rangeSize);
    rangeSym->setDepth(rangeSize);

    if (i * rangeMultiplier >= maxRange)
      distanceCompSymbol->ranges()->append(new DistanceSymbolRange(rangeSym, i, 0.0, this));
    else
      distanceCompSymbol->ranges()->append(new DistanceSymbolRange(rangeSym, i, i * rangeMultiplier, this));
  }

  SimpleRenderer* renderer = new SimpleRenderer(distanceCompSymbol, this);
  RendererSceneProperties renderProperties = renderer->sceneProperties();
  renderProperties.setHeadingExpression(QString("[heading]"));
  renderer->setSceneProperties(renderProperties);

  m_locationOverlay = new GraphicsOverlay(this);
  m_locationOverlay->setOverlayId(QStringLiteral("SCENEVIEWLOCATIONOVERLAY"));
  m_locationOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
  m_locationOverlay->setRenderingMode(GraphicsRenderingMode::Dynamic);
  m_locationOverlay->setRenderer(renderer);

  m_positionGraphic = new Graphic(this);
  m_positionGraphic->attributes()->insertAttribute("heading", 0.0);
  m_locationOverlay->graphics()->append(m_positionGraphic);

  connect(this, &LocationController::positionChanged, this, [this](const Point& newPosition)
  {
    constexpr double z = 10.0;
    m_positionGraphic->setGeometry(Point(newPosition.x(), newPosition.y(), z));
  });

  auto attributes = m_positionGraphic->attributes();
  connect(this, &LocationController::headingChanged, this, [this, attributes](double newHeading)
  {
    attributes->replaceAttribute("heading", newHeading);
  });
}

void LocationController::setIconDataPath(const QString& dataPath)
{
  if (dataPath == m_iconDataPath)
    return;

  m_iconDataPath = dataPath;
  emit propertyUpdated(RESOURCE_DIRECTORY_PROPERTYNAME, m_iconDataPath);
}

QUrl LocationController::modelSymbolPath() const
{
  // both files are needed: LocationDisplay.dae
  // and navigation.png and both must be local (not resources)  
  QString modelPath = m_iconDataPath + "/LocationDisplay.dae";
  QString imagePath = m_iconDataPath + "/navigation.png";

  if (QFile::exists(modelPath) && QFile::exists(imagePath))
    return QUrl::fromLocalFile(modelPath);

  const QString tempPath = QDir::tempPath();
  modelPath = tempPath + "/LocationDisplay.dae";
  imagePath = tempPath + "/navigation.png";

  // check if we've already copied them to temp
  if (QFile::exists(modelPath) && QFile::exists(imagePath))
    return QUrl::fromLocalFile(modelPath);

  // if they're not both available, save both from resources to temp
  // and access from there
  QFile modelResource(":Resources/LocationDisplay.dae");
  QFile imageResource(":Resources/icons/xhdpi/navigation.png");

  modelResource.open(QIODevice::ReadOnly);
  imageResource.open(QIODevice::ReadOnly);

  QFile modelFileTemp(modelPath);
  QFile imageFileTemp(imagePath);

  modelFileTemp.open(QIODevice::WriteOnly);
  imageFileTemp.open(QIODevice::WriteOnly);

  modelFileTemp.write(modelResource.readAll());
  imageFileTemp.write(imageResource.readAll());

  for (QFile* file : { &modelResource, &imageResource, &modelFileTemp, &imageFileTemp })
    file->close();

  return QUrl::fromLocalFile(modelPath);
}
