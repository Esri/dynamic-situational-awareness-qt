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

#ifndef LOCATIONCONTROLLER_H
#define LOCATIONCONTROLLER_H

#include <QUrl>

#include "AbstractTool.h"
#include "Point.h"

namespace Esri {
namespace ArcGISRuntime
{
  class SceneQuickView;
  class GraphicsOverlay;
}}

class QGeoPositionInfoSource;
class QCompass;
class GPXLocationSimulator;
class LocationDisplay3d;

class LocationController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(bool locationVisible READ isLocationVisible WRITE setLocationVisible NOTIFY locationVisibleChanged)
  Q_PROPERTY(bool simulated READ isSimulated WRITE setSimulated NOTIFY simulatedChanged)
  Q_PROPERTY(QUrl gpxFilePath READ gpxFilePath WRITE setGpxFilePath NOTIFY gpxFilePathChanged)
  Q_PROPERTY(QString gpxFilePathAsString READ gpxFilePathAsString NOTIFY gpxFilePathChanged)

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

  bool isSimulated() const;
  void setSimulated(bool isSimulated);

  Esri::ArcGISRuntime::Point currentLocation() const;

  QUrl gpxFilePath() const;
  void setGpxFilePath(const QUrl& gpxFilePath);

  QString iconDataPath() const { return m_iconDataPath; }
  void setIconDataPath(const QString& dataPath);

  // removes any file:// scheme if present
  QString gpxFilePathAsString() const;

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
  void simulatedChanged();

  // see setRelativeHeadingSceneView
  void relativeHeadingChanged(double relativeHeading);

private slots:
  void updateGeoView();

private:
  void initPositionInfoSource(bool isSimulated);
  void clearPositionInfoSource();
  QUrl modelSymbolPath() const;

  QGeoPositionInfoSource* m_positionSource = nullptr;
  QCompass* m_compass = nullptr;
  LocationDisplay3d* m_locationDisplay3d = nullptr;
  bool m_enabled = false;
  bool m_simulated = false;
  double m_lastViewHeading = 0.0;
  double m_lastKnownHeading = 0.0;
  Esri::ArcGISRuntime::Point m_currentLocation;
  QUrl m_gpxFilePath;
  QString m_iconDataPath;
};

#endif // LOCATIONCONTROLLER_H
