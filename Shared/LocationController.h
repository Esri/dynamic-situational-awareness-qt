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

#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// C++ API headers
#include "Point.h"

// Qt headers
#include <QString>

namespace Esri {
namespace ArcGISRuntime {
  class SceneQuickView;
  class GraphicsOverlay;
}}

class QGeoPositionInfoSource;
class QCompass;

namespace Dsa {

class GPXLocationSimulator;
class LocationDisplay3d;

class LocationController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(bool locationVisible READ isLocationVisible WRITE setLocationVisible NOTIFY locationVisibleChanged)
  Q_PROPERTY(bool simulationEnabled READ isSimulationEnabled WRITE setSimulationEnabled NOTIFY simulationEnabledChanged)
  Q_PROPERTY(QString gpxFilePath READ gpxFilePath WRITE setGpxFilePath NOTIFY gpxFilePathChanged)

public:
  static const QString SIMULATE_LOCATION_PROPERTYNAME;
  static const QString GPX_FILE_PROPERTYNAME;
  static const QString RESOURCE_DIRECTORY_PROPERTYNAME;

  explicit LocationController(QObject* parent = nullptr);
  ~LocationController();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  bool isEnabled() const;
  void setEnabled(bool isEnabled);

  bool isLocationVisible() const;
  void setLocationVisible(bool isVisible);

  bool isSimulationEnabled() const;
  void setSimulationEnabled(bool isSimulationEnabled);

  Esri::ArcGISRuntime::Point currentLocation() const;

  LocationDisplay3d* locationDisplay() const;

  QString gpxFilePath() const;
  void setGpxFilePath(const QString& gpxFilePath);

  QString iconDataPath() const { return m_iconDataPath; }
  void setIconDataPath(const QString& dataPath);

  // if using GraphicsRenderingMode::Dynamic for rendering, then we need to massage
  // the heading value to make sure it's correct when the scene is rotated.
  // Set the sceneView here and connect to relativeHeadingChanged and that will
  // be handled automatically.
  // Note: this is only needed for PictureMarkerSymbol. ModelSceneSybol already
  // takes this into account
  void setRelativeHeadingSceneView(Esri::ArcGISRuntime::SceneQuickView* sceneView);

signals:
  void locationChanged(const Esri::ArcGISRuntime::Point& newLocation);
  void headingChanged(double newHeading);
  void gpxFilePathChanged();
  void enabledChanged();
  void locationVisibleChanged();
  void simulationEnabledChanged();
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

  // see setRelativeHeadingSceneView
  void relativeHeadingChanged(double relativeHeading);

private slots:
  void updateGeoView();

private:
  void initPositionInfoSource(bool isSimulationEnabled);
  void clearPositionInfoSource();
  QUrl modelSymbolPath() const;

  QGeoPositionInfoSource* m_positionSource = nullptr;
  QCompass* m_compass = nullptr;
  LocationDisplay3d* m_locationDisplay3d = nullptr;
  bool m_enabled = false;
  bool m_simulated = true;
  double m_lastViewHeading = 0.0;
  double m_lastKnownHeading = 0.0;
  Esri::ArcGISRuntime::Point m_currentLocation;
  QString m_gpxFilePath;
  QString m_iconDataPath;
};

} // Dsa

#endif // LOCATIONCONTROLLER_H
