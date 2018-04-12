// Copyright 2018 ESRI
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

// toolkit headers
#include "AbstractTool.h"

namespace Esri {
namespace ArcGISRuntime {
  class CameraController;
  class GeoElement;
  class GeoView;
  class GraphicListModel;
}}

namespace Dsa {

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

  void followGeoElement(Esri::ArcGISRuntime::GeoElement* elementToFollow);

signals:
  void followModeChanged();

private slots:
  void updateGeoView();

private:

  void handleNewMode();
  bool handleFollowInMap();
  bool handleFollowInScene();
  Esri::ArcGISRuntime::GraphicListModel* locationGraphicsModel() const;

  FollowMode m_mode = FollowMode::Disabled;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
};

} // Dsa

#endif // FOLLOWPOSITIONCONTROLLER_H
