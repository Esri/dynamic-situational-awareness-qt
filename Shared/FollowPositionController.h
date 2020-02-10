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

class FollowPositionController : public AbstractTool
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
