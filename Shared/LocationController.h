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

namespace Esri {
namespace ArcGISRuntime
{
  class Point;
  class SceneQuickView;
  class Graphic;
  class GraphicsOverlay;
}}

class QGeoPositionInfoSource;
class QCompass;
class GPXLocationSimulator;

class LocationController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(bool simulated READ isSimulated WRITE setSimulated NOTIFY simulatedChanged)
  Q_PROPERTY(QUrl gpxFilePath READ gpxFilePath WRITE setGpxFilePath NOTIFY gpxFilePathChanged)
  Q_PROPERTY(QString gpxFilePathAsString READ gpxFilePathAsString NOTIFY gpxFilePathChanged)

public:
  LocationController(QObject* parent = nullptr);
  ~LocationController();

  QString toolName() const override;

  // if you want this tool to handle everything, add this overlay
  // to your view
  Esri::ArcGISRuntime::GraphicsOverlay* locationOverlay();

  bool isEnabled() const;
  void setEnabled(bool isEnabled);

  bool isSimulated() const;
  void setSimulated(bool isSimulated);

  QUrl gpxFilePath() const;
  void setGpxFilePath(const QUrl& gpxFilePath);

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
  void positionChanged(const Esri::ArcGISRuntime::Point& newPosition);
  void headingChanged(double newHeading);
  void gpxFilePathChanged();
  void enabledChanged();
  void simulatedChanged();

  // see setRelativeHeadingSceneView
  void relativeHeadingChanged(double relativeHeading);

private:
  void initPositionInfoSource(bool isSimulated);
  void initOverlay();
  QUrl modelSymbolPath() const;

  Esri::ArcGISRuntime::GraphicsOverlay* m_locationOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_positionGraphic = nullptr;
  GPXLocationSimulator* m_simulator = nullptr;
  QGeoPositionInfoSource* m_positionSource = nullptr;
  QCompass* m_compass = nullptr;
  bool m_enabled = false;
  bool m_simulated = false;
  double m_lastViewHeading = 0.0;
  double m_lastKnownHeading = 0.0;
  QUrl m_gpxFilePath;
};

#endif // LOCATIONCONTROLLER_H
