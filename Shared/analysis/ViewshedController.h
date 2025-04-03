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

#ifndef VIEWSHEDCONTROLLER_H
#define VIEWSHEDCONTROLLER_H

// Qt headers
#include <qtmetamacros.h>
class QAbstractListModel;
Q_MOC_INCLUDE("qabstractitemmodel.h")

// DSA headers
#include "AbstractTool.h"

class QMouseEvent;

namespace Esri::ArcGISRuntime {
  class SceneView;
  class AnalysisOverlay;
  class GeoElement;
  class GlobeCameraController;
  class GraphicsOverlay;
  class OrbitLocationCameraController;
}

namespace Dsa {

class ViewshedListModel;
class Viewshed360;
class GeoElementViewshed360;

class ViewshedController : public AbstractTool
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
  Q_PROPERTY(bool locationDisplayViewshedActive READ isLocationDisplayViewshedActive NOTIFY locationDisplayViewshedActiveChanged)

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
  void locationDisplayViewshedActiveChanged();

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

  ViewshedActiveMode m_activeMode = ViewshedActiveMode::NoActiveMode;

  QList<QMetaObject::Connection> m_activeViewshedConns;
};

} // Dsa

#endif // VIEWSHEDCONTROLLER_H
