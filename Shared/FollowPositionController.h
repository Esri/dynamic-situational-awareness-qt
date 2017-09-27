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

#ifndef FOLLOWPOSITIONCONTROLLER_H
#define FOLLOWPOSITIONCONTROLLER_H

#include <QUrl>

#include "AbstractTool.h"

namespace Esri {
namespace ArcGISRuntime
{
  class CameraController;
  class GeoView;
  class GraphicListModel;
}}

class FollowPositionController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(FollowMode followMode READ followMode WRITE setFollowMode NOTIFY followModeChanged)

public:

  enum class FollowMode
  {
    Disabled = 0,
    TrackUp = 1,
    NorthUp = 2
  };

  Q_ENUM(FollowMode)

  FollowPositionController(QObject* parent = nullptr);
  ~FollowPositionController();

  void init(Esri::ArcGISRuntime::GeoView* geoView);

  FollowMode followMode() const;
  void setFollowMode(FollowMode followMode);

  // AbstractTool interface
  QString toolName() const override;

signals:
  void followModeChanged();

private:

  void handleNewMode();
  bool handleFollowInMap();
  bool handleFollowInScene();
  Esri::ArcGISRuntime::GraphicListModel* locationGraphicsModel() const;

  FollowMode m_mode = FollowMode::Disabled;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
};

#endif // FOLLOWPOSITIONCONTROLLER_H
