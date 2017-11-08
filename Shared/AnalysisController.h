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

#ifndef ANALYSISCONTROLLER_H
#define ANALYSISCONTROLLER_H

#include "AbstractTool.h"

#include <QPoint>

namespace Esri {
  namespace ArcGISRuntime {
    class AnalysisOverlay;
    class LocationViewshed;
    class GraphicsOverlay;
    class Graphic;
  }
}

class AnalysisController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool viewshedEnabled READ isViewshedEnabled WRITE setViewshedEnabled NOTIFY viewshedEnabledChanged)
  Q_PROPERTY(bool viewshedVisible READ isViewshedVisible WRITE setViewshedVisible NOTIFY viewshedVisibleChanged)
  Q_PROPERTY(double minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(double maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(double horizontalAngle READ horizontalAngle WRITE setHorizontalAngle NOTIFY horizontalAngleChanged)
  Q_PROPERTY(double verticalAngle READ verticalAngle WRITE setVerticalAngle NOTIFY verticalAngleChanged)
  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)
  Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)

signals:
  void viewshedEnabledChanged();
  void viewshedVisibleChanged();
  void minDistanceChanged();
  void maxDistanceChanged();
  void horizontalAngleChanged();
  void verticalAngleChanged();
  void headingChanged();
  void pitchChanged();

public:
  explicit AnalysisController(QObject* parent = nullptr);
  ~AnalysisController();

  Q_INVOKABLE void removeViewshed();

  bool isViewshedEnabled() const;
  void setViewshedEnabled(bool viewshedEnabled);

  bool isViewshedVisible() const;
  void setViewshedVisible(bool viewshedVisible);

  double minDistance() const;
  void setMinDistance(double minDistance);

  double maxDistance() const;
  void setMaxDistance(double maxDistance);

  double horizontalAngle() const;
  void setHorizontalAngle(double horizontalAngle);

  double verticalAngle() const;
  void setVerticalAngle(double verticalAngle);

  double heading() const;
  void setHeading(double heading);

  double pitch() const;
  void setPitch(double pitch);

  QString toolName() const override;

private:
  void connectMouseSignals();

  bool m_viewshedEnabled = false;

  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::LocationViewshed* m_locationViewshed = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_locationViewshedGraphic = nullptr;

  bool m_viewshedVisibleDefault = true;
  double m_minDistanceDefault = 50;
  double m_maxDistanceDefault = 1000;
  double m_horizontalAngleDefault = 45;
  double m_verticalAngleDefault = 90;
  double m_headingDefault = 0;
  double m_pitchDefault = 90;
};

#endif // ANALYSISCONTROLLER_H
