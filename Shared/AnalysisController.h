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

#include <QAbstractListModel>

namespace Esri {
  namespace ArcGISRuntime {
    class AnalysisOverlay;
    class GraphicsOverlay;
  }
}

//class AbstractViewshed;
class ViewshedListModel;
class GraphicViewshed;
class QMouseEvent;

class AnalysisController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool viewshedEnabled READ isViewshedEnabled WRITE setViewshedEnabled NOTIFY viewshedEnabledChanged)
  Q_PROPERTY(bool locationDisplayViewshedActive READ isLocationDisplayViewshedActive NOTIFY locationDisplayViewshedActiveChanged)
  Q_PROPERTY(AnalysisActiveMode analysisActiveMode READ analysisActiveMode WRITE setAnalysisActiveMode NOTIFY analysisActiveModeChanged)
//  Q_PROPERTY(int analysisType READ analysisType NOTIFY analysisTypeChanged)
//  Q_PROPERTY(bool viewshedVisible READ isViewshedVisible WRITE setViewshedVisible NOTIFY viewshedVisibleChanged)
//  Q_PROPERTY(double minDistance READ minDistance WRITE setMinDistance NOTIFY minDistanceChanged)
//  Q_PROPERTY(double maxDistance READ maxDistance WRITE setMaxDistance NOTIFY maxDistanceChanged)
//  Q_PROPERTY(double horizontalAngle READ horizontalAngle WRITE setHorizontalAngle NOTIFY horizontalAngleChanged)
//  Q_PROPERTY(double verticalAngle READ verticalAngle WRITE setVerticalAngle NOTIFY verticalAngleChanged)
//  Q_PROPERTY(double heading READ heading WRITE setHeading NOTIFY headingChanged)
//  Q_PROPERTY(double pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
//  Q_PROPERTY(bool viewshed360Mode READ isViewshed360Mode WRITE setViewshed360Mode NOTIFY viewshed360ModeChanged)
//  Q_PROPERTY(int viewshedIndex READ viewshedIndex WRITE setViewshedIndex NOTIFY viewshedIndexChanged)
  Q_PROPERTY(ViewshedListModel* viewsheds READ viewsheds CONSTANT)

signals:
  void viewshedEnabledChanged();
  void locationDisplayViewshedActiveChanged();
  void analysisActiveModeChanged();
//  void analysisTypeChanged();
//  void viewshedVisibleChanged();
//  void viewshedTypeIndexChanged();
//  void minDistanceChanged();
//  void maxDistanceChanged();
//  void horizontalAngleChanged();
//  void verticalAngleChanged();
//  void headingChanged();
//  void pitchChanged();
//  void viewshed360ModeChanged();
//  void viewshedIndexChanged();

public:
  enum AnalysisActiveMode
  {
    None = 0,
    AddMapPointViewshed,
    AddMessageFeedViewshed,
    EditMapPointViewshed
  };

  Q_ENUM(AnalysisActiveMode)

  explicit AnalysisController(QObject* parent = nullptr);
  ~AnalysisController();

  Q_INVOKABLE void addLocationDisplayViewshed();
  //Q_INVOKABLE void removeViewshed();

  bool isViewshedEnabled() const;
  void setViewshedEnabled(bool viewshedEnabled);

  bool isLocationDisplayViewshedActive() const;

  AnalysisActiveMode analysisActiveMode() const;
  void setAnalysisActiveMode(AnalysisActiveMode mode);

//  int analysisType() const;

//  bool isViewshedVisible() const;
//  void setViewshedVisible(bool viewshedVisible);

//  double minDistance() const;
//  void setMinDistance(double minDistance);

//  double maxDistance() const;
//  void setMaxDistance(double maxDistance);

//  double horizontalAngle() const;
//  void setHorizontalAngle(double horizontalAngle);

//  double verticalAngle() const;
//  void setVerticalAngle(double verticalAngle);

//  double heading() const;
//  void setHeading(double heading);

//  double pitch() const;
//  void setPitch(double pitch);

//  bool isViewshed360Mode() const;
//  void setViewshed360Mode(bool viewshed360Mode);

//  int viewshedIndex() const;
//  void setViewshedIndex(int index);

  ViewshedListModel* viewsheds() const;

  QString toolName() const override;

private:
  void updateGeoView();
  void connectMouseSignals();

  void addMapPointViewshed(QMouseEvent& event);
  void addMessageFeedViewshed(QMouseEvent& event);

  //void emitAllChanged();

  bool m_viewshedEnabled = false;

  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  //AbstractViewshed* m_currentViewshed = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  ViewshedListModel* m_viewsheds = nullptr;
  GraphicViewshed* m_locationDisplayViewshed = nullptr;

  AnalysisActiveMode m_activeMode = AnalysisActiveMode::None;
//  int m_viewshedIndex = -1;

  Esri::ArcGISRuntime::TaskWatcher m_identifyTaskWatcher;
  QMetaObject::Connection m_identifyConn;
};

#endif // ANALYSISCONTROLLER_H
