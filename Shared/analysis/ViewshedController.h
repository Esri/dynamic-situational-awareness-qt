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

#ifndef VIEWSHEDCONTROLLER_H
#define VIEWSHEDCONTROLLER_H


// toolkit headers
#include "AbstractTool.h"

// C++ API headers
#include "TaskWatcher.h"

// Qt headers
#include <QAbstractListModel>

namespace Esri {
  namespace ArcGISRuntime {
    class SceneView;
    class AnalysisOverlay;
    class GeoElement;
    class GlobeCameraController;
    class GraphicsOverlay;
    class OrbitLocationCameraController;
  }
}

class ViewshedListModel;
class Viewshed360;
class GeoElementViewshed360;
class QMouseEvent;

class ViewshedController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(ViewshedActiveMode activeMode READ activeMode WRITE setActiveMode NOTIFY activeModeChanged)
  Q_PROPERTY(QAbstractListModel* viewsheds READ viewsheds CONSTANT)

  // active viewshed properties
  Q_PROPERTY(bool activeViewshedEnabled READ isActiveViewshedEnabled NOTIFY activeViewshedEnabledChanged)
  Q_PROPERTY(double activeViewshedMinDistance READ activeViewshedMinDistance WRITE setActiveViewshedMinDistance NOTIFY activeViewshedMinDistanceChanged)
  Q_PROPERTY(double activeViewshedMaxDistance READ activeViewshedMaxDistance WRITE setActiveViewshedMaxDistance NOTIFY activeViewshedMaxDistanceChanged)
  Q_PROPERTY(double activeViewshedHorizontalAngle READ activeViewshedHorizontalAngle WRITE setActiveViewshedHorizontalAngle NOTIFY activeViewshedHorizontalAngleChanged)
  Q_PROPERTY(double activeViewshedVerticalAngle READ activeViewshedVerticalAngle WRITE setActiveViewshedVerticalAngle NOTIFY activeViewshedVerticalAngleChanged)
  Q_PROPERTY(double activeViewshedHeading READ activeViewshedHeading WRITE setActiveViewshedHeading NOTIFY activeViewshedHeadingChanged)
  Q_PROPERTY(double activeViewshedPitch READ activeViewshedPitch WRITE setActiveViewshedPitch NOTIFY activeViewshedPitchChanged)
  Q_PROPERTY(double activeViewshedMinPitch READ activeViewshedMinPitch NOTIFY activeViewshedMinPitchChanged)
  Q_PROPERTY(double activeViewshedMaxPitch READ activeViewshedMaxPitch NOTIFY activeViewshedMaxPitchChanged)
  Q_PROPERTY(double activeViewshedOffsetZ READ activeViewshedOffsetZ WRITE setActiveViewshedOffsetZ NOTIFY activeViewshedOffsetZChanged)
  Q_PROPERTY(bool activeViewshedHeadingEnabled READ isActiveViewshedHeadingEnabled NOTIFY activeViewshedHeadingEnabledChanged)
  Q_PROPERTY(bool activeViewshedPitchEnabled READ isActiveViewshedPitchEnabled NOTIFY activeViewshedPitchEnabledChanged)
  Q_PROPERTY(bool activeViewshedOffsetZEnabled READ isActiveViewshedOffsetZEnabled NOTIFY activeViewshedOffsetZEnabledChanged)
  Q_PROPERTY(bool activeViewshed360Mode READ isActiveViewshed360Mode WRITE setActiveViewshed360Mode NOTIFY activeViewshed360ModeChanged)

signals:
  void activeModeChanged();

  // active viewshed signals
  void activeViewshedEnabledChanged();
  void activeViewshedMinDistanceChanged();
  void activeViewshedMaxDistanceChanged();
  void activeViewshedHorizontalAngleChanged();
  void activeViewshedVerticalAngleChanged();
  void activeViewshedHeadingChanged();
  void activeViewshedPitchChanged();
  void activeViewshedMinPitchChanged();
  void activeViewshedMaxPitchChanged();
  void activeViewshedOffsetZChanged();
  void activeViewshedHeadingEnabledChanged();
  void activeViewshedPitchEnabledChanged();
  void activeViewshedOffsetZEnabledChanged();
  void activeViewshed360ModeChanged();

public:
  enum ViewshedActiveMode
  {
    NoActiveMode = 0,
    AddMyLocationViewshed360,
    AddLocationViewshed360,
    AddGeoElementViewshed360
  };

  Q_ENUM(ViewshedActiveMode)

  static const QString VIEWSHED_HEADING_ATTRIBUTE;
  static const QString VIEWSHED_PITCH_ATTRIBUTE;

  explicit ViewshedController(QObject* parent = nullptr);
  ~ViewshedController();

  void setSceneView(Esri::ArcGISRuntime::SceneView* sceneView);

  void addLocationDisplayViewshed();
  void addLocationViewshed360(const Esri::ArcGISRuntime::Point& point);
  void addGeoElementViewshed360(Esri::ArcGISRuntime::GeoElement* geoElement);

  bool isLocationDisplayViewshedActive() const;

  ViewshedActiveMode activeMode() const;
  void setActiveMode(ViewshedActiveMode mode);

  QAbstractListModel* viewsheds() const;

  QString toolName() const override;

  // active viewshed methods
  Viewshed360* activeViewshed() const;
  Q_INVOKABLE void removeActiveViewshed();
  Q_INVOKABLE void finishActiveViewshed();

  bool isActiveViewshedEnabled() const;

  int activeViewshedIndex() const;
  void setActiveViewshedIndex(int index);

  double activeViewshedMinDistance() const;
  void setActiveViewshedMinDistance(double minDistance);

  double activeViewshedMaxDistance() const;
  void setActiveViewshedMaxDistance(double maxDistance);

  double activeViewshedHorizontalAngle() const;
  void setActiveViewshedHorizontalAngle(double horizontalAngle);

  double activeViewshedVerticalAngle() const;
  void setActiveViewshedVerticalAngle(double verticalAngle);

  double activeViewshedHeading() const;
  void setActiveViewshedHeading(double heading);

  double activeViewshedPitch() const;
  void setActiveViewshedPitch(double pitch);

  double activeViewshedMinPitch() const;
  double activeViewshedMaxPitch() const;

  double activeViewshedOffsetZ() const;
  void setActiveViewshedOffsetZ(double offsetZ);

  bool isActiveViewshedHeadingEnabled() const;
  bool isActiveViewshedPitchEnabled() const;
  bool isActiveViewshedOffsetZEnabled() const;

  bool isActiveViewshed360Mode() const;
  void setActiveViewshed360Mode(bool is360Mode);

public slots:
  void onMouseClicked(QMouseEvent& event);
  void onMouseMoved(QMouseEvent& event);

private:
  void connectMouseSignals();

  void updateActiveViewshed();
  void updateActiveViewshedSignals();
  void disconnectActiveViewshedSignals();
  void emitActiveViewshedSignals();

  Esri::ArcGISRuntime::SceneView* m_sceneView = nullptr;

  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::OrbitLocationCameraController* m_followCamCtrllr = nullptr;
  Esri::ArcGISRuntime::GlobeCameraController* m_navCamCtrllr = nullptr;

  ViewshedListModel* m_viewsheds = nullptr;
  Viewshed360* m_activeViewshed = nullptr;
  GeoElementViewshed360* m_locationDisplayViewshed = nullptr;

  int m_activeViewshedIndex = -1;
  ViewshedActiveMode m_activeMode = ViewshedActiveMode::NoActiveMode;

  Esri::ArcGISRuntime::TaskWatcher m_identifyTaskWatcher;
  QMetaObject::Connection m_identifyConn;

  QList<QMetaObject::Connection> m_activeViewshedConns;
};

#endif // VIEWSHEDCONTROLLER_H
