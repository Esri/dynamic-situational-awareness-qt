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

#include "AbstractTool.h"
#include "TaskWatcher.h"

namespace Esri {
  namespace ArcGISRuntime {
    class SceneView;
    class AnalysisOverlay;
    class GraphicsOverlay;
    class Graphic;
  }
}

class ViewshedListModel;
class GraphicViewshed;
class QMouseEvent;

class ViewshedController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool locationDisplayViewshedActive READ isLocationDisplayViewshedActive NOTIFY locationDisplayViewshedActiveChanged)
  Q_PROPERTY(ViewshedActiveMode activeMode READ activeMode WRITE setActiveMode NOTIFY activeModeChanged)
  Q_PROPERTY(ViewshedListModel* viewsheds READ viewsheds CONSTANT)

signals:
  void locationDisplayViewshedActiveChanged();
  void activeModeChanged();

public:
  enum ViewshedActiveMode
  {
    NoActiveMode = 0,
    AddMapPointViewshed,
    AddMessageFeedViewshed
  };

  Q_ENUM(ViewshedActiveMode)

  explicit ViewshedController(QObject* parent = nullptr);
  ~ViewshedController();

  void setSceneView(Esri::ArcGISRuntime::SceneView* sceneView);

  Q_INVOKABLE void addLocationDisplayViewshed();
  void addMapPointViewshed(const Esri::ArcGISRuntime::Point& point);
  void addMessageFeedViewshed(Esri::ArcGISRuntime::Graphic* graphic);

  bool isLocationDisplayViewshedActive() const;

  ViewshedActiveMode activeMode() const;
  void setActiveMode(ViewshedActiveMode mode);

  ViewshedListModel* viewsheds() const;

  QString toolName() const override;

private:
  void connectMouseSignals();

  Esri::ArcGISRuntime::SceneView* m_sceneView = nullptr;

  Esri::ArcGISRuntime::AnalysisOverlay* m_analysisOverlay = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;

  ViewshedListModel* m_viewsheds = nullptr;
  GraphicViewshed* m_locationDisplayViewshed = nullptr;

  ViewshedActiveMode m_activeMode = ViewshedActiveMode::NoActiveMode;

  Esri::ArcGISRuntime::TaskWatcher m_identifyTaskWatcher;
  QMetaObject::Connection m_identifyConn;
};

#endif // VIEWSHEDCONTROLLER_H
