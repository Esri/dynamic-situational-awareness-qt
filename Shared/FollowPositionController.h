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

  Q_PROPERTY(bool follow READ isFollow WRITE setFollow NOTIFY followChanged)
  Q_PROPERTY(bool northUp READ isNorthUp WRITE setNorthUp NOTIFY northUpChanged)

public:
  FollowPositionController(QObject* parent = nullptr);
  ~FollowPositionController();

  void init(Esri::ArcGISRuntime::GeoView* geoView);

  void setFollow(bool follow);
  bool isFollow() const;

  void setNorthUp(bool northUp);
  bool isNorthUp() const;

  // AbstractTool interface
  QString toolName() const override;

signals:
  void followChanged();
  void northUpChanged();

private:
  bool handleFollowInMap();
  bool handleFollowInScene();
  Esri::ArcGISRuntime::GraphicListModel* locationGraphicsModel() const;

  bool m_following = false;
  bool m_northUp = true;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
};

#endif // FOLLOWPOSITIONCONTROLLER_H
