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
    class GeoView;
    class LocationViewshed;
    class SceneQuickView;
  }
}

class AnalysisController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool analysisVisible READ isAnalysisVisible WRITE setAnalysisVisible NOTIFY analysisVisibleChanged)
  Q_PROPERTY(double minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
  Q_PROPERTY(double maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
  Q_PROPERTY(double horizontalAngle READ horizontalAngle WRITE setHorizontalAngle NOTIFY horizontalAngleChanged)
  Q_PROPERTY(double verticalAngle READ verticalAngle WRITE setVerticalAngle NOTIFY verticalAngleChanged)
  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)

signals:
  void analysisVisibleChanged();
  void minDistanceChanged();
  void maxDistanceChanged();
  void horizontalAngleChanged();
  void verticalAngleChanged();
  void headingChanged();

public:
  explicit AnalysisController(QObject* parent = nullptr);
  ~AnalysisController();

  void init(Esri::ArcGISRuntime::GeoView* geoView);

  bool isAnalysisVisible() const;
  void setAnalysisVisible(bool analysisVisible);

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

  QString toolName() const override;

private:
  bool m_viewshedActive = false;
  bool m_analysisVisible = false;

  Esri::ArcGISRuntime::SceneQuickView* m_sceneView = nullptr;
  Esri::ArcGISRuntime::LocationViewshed* m_locationViewshed = nullptr;

  Qt::MouseButton m_currentMouseButton = Qt::NoButton;
  QPoint m_pressedPoint;
};

#endif // ANALYSISCONTROLLER_H
