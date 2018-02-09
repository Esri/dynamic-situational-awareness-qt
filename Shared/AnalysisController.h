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

#ifndef ANALYSISCONTROLLER_H
#define ANALYSISCONTROLLER_H

#include "AbstractTool.h"
#include "TaskWatcher.h"

#include <QPoint>

namespace Esri {
  namespace ArcGISRuntime {
    class AnalysisOverlay;
    class Viewshed;
    class GraphicsOverlay;
    class Graphic;
  }
}

class QMouseEvent;

class AnalysisController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool viewshedEnabled READ isViewshedEnabled WRITE setViewshedEnabled NOTIFY viewshedEnabledChanged)
  Q_PROPERTY(bool viewshedVisible READ isViewshedVisible WRITE setViewshedVisible NOTIFY viewshedVisibleChanged)
  Q_PROPERTY(QStringList viewshedTypes READ viewshedTypes CONSTANT)
  Q_PROPERTY(int viewshedTypeIndex READ viewshedTypeIndex WRITE setViewshedTypeIndex NOTIFY viewshedTypeIndexChanged)
  Q_PROPERTY(double minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(double maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(double horizontalAngle READ horizontalAngle WRITE setHorizontalAngle NOTIFY horizontalAngleChanged)
  Q_PROPERTY(double verticalAngle READ verticalAngle WRITE setVerticalAngle NOTIFY verticalAngleChanged)
  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)
  Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)

signals:
  void viewshedEnabledChanged();
  void viewshedVisibleChanged();
  void viewshedTypeIndexChanged();
  void minDistanceChanged();
  void maxDistanceChanged();
  void horizontalAngleChanged();
  void verticalAngleChanged();
  void headingChanged();
  void pitchChanged();

public:
  static const QString MAP_POINT_VIEWSHED_TYPE;
  static const QString MY_LOCATION_VIEWSHED_TYPE;
  static const QString FRIENDLY_TRACK_VIEWSHED_TYPE;

  explicit AnalysisController(QObject* parent = nullptr);
  ~AnalysisController();

  Q_INVOKABLE void removeViewshed();

  bool isViewshedEnabled() const;
  void setViewshedEnabled(bool viewshedEnabled);

  bool isViewshedVisible() const;
  void setViewshedVisible(bool viewshedVisible);

  QStringList viewshedTypes() const;

  int viewshedTypeIndex() const;
  void setViewshedTypeIndex(int index);

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
  void updateGeoView();
  void connectMouseSignals();

  void updateCurrentViewshed();
  void updateMapPointViewshed(QMouseEvent& event);
  void updateMyLocationViewshed();
  void updateFriendlyTrackViewshed(QMouseEvent& event);

  bool m_viewshedEnabled = false;

  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::Viewshed* m_currentViewshed = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::Graphic* m_locationViewshedGraphic = nullptr;
  QList<Esri::ArcGISRuntime::Viewshed*> m_viewsheds;

  bool m_viewshedVisibleDefault = true;
  QStringList m_viewshedTypes;
  int m_viewshedTypeIndex = 0;
  double m_minDistanceDefault = 50;
  double m_maxDistanceDefault = 1000;
  double m_horizontalAngleDefault = 120;
  double m_verticalAngleDefault = 90;
  double m_headingDefault = 0;
  double m_pitchDefault = 90;

  Esri::ArcGISRuntime::TaskWatcher m_identifyTaskWatcher;
  QMetaObject::Connection m_identifyConn;
};

#endif // ANALYSISCONTROLLER_H
