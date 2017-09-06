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

#include "ArcGISTiledLayer.h"
#include "Basemap.h"
#include "Scene.h"
#include "SceneQuickView.h"

#include "../Shared/DsaUtility.h"

#include "Handheld.h"

using namespace Esri::ArcGISRuntime;

Handheld::Handheld(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
  m_dataPath = DsaUtility::dataPath();
}

Handheld::~Handheld()
{
}

void Handheld::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");

  // Create a scene using the light grey canvas tile package
  TileCache* tileCache = new TileCache(m_dataPath + QStringLiteral("/LightGreyCanvas.tpk"), this);
  m_scene = new Scene(new Basemap(new ArcGISTiledLayer(tileCache, this), this), this);

  // Set scene to scene view
  m_sceneView->setArcGISScene(m_scene);

  // Set viewpoint to Monterey, CA
  // distance of 5000m, heading North, pitch at 30 degrees, roll of 0
  Camera monterey(DsaUtility::montereyCA(), 5000, 0., 30., 0);
  m_sceneView->setViewpointCamera(monterey);
}
