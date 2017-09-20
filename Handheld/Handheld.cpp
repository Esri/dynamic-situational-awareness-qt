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

#include "SceneQuickView.h"
#include "ArcGISCompassController.h"

#include "DsaController.h"
#include "Handheld.h"

using namespace Esri::ArcGISRuntime;

Handheld::Handheld(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_controller(new DsaController(this))
{

}

Handheld::~Handheld()
{
}

void Handheld::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  connect(m_sceneView, &SceneQuickView::errorOccurred, m_controller, &DsaController::onError);

  m_controller->init(m_sceneView);

  // Set scene to scene view
  m_sceneView->setArcGISScene(m_controller->scene());
}
