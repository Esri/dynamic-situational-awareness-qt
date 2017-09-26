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

  Q_PROPERTY(bool trackUp READ isTrackUp NOTIFY modeChanged)
  Q_PROPERTY(bool northUp READ isNorthUp NOTIFY modeChanged)

public:
  FollowPositionController(QObject* parent = nullptr);
  ~FollowPositionController();

  void init(Esri::ArcGISRuntime::GeoView* geoView);

  Q_INVOKABLE void setDisabled();

  bool isTrackUp() const;
  Q_INVOKABLE void setTrackUp();

  bool isNorthUp() const;
  Q_INVOKABLE void setNorthUp();

  // AbstractTool interface
  QString toolName() const override;

signals:
  void modeChanged();

private:

  enum class Mode
  {
    Disabled = 0,
    TrackUp = 1,
    NorthUp = 2
  };

  void handleNewMode();
  bool handleFollowInMap();
  bool handleFollowInScene();
  Esri::ArcGISRuntime::GraphicListModel* locationGraphicsModel() const;

  Mode m_mode = Mode::Disabled;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
};

#endif // FOLLOWPOSITIONCONTROLLER_H
