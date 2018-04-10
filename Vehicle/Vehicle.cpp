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

// PCH header
#include "pch.hpp"

#include "Vehicle.h"

// example app headers
#include "DsaController.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "SceneQuickView.h"

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

namespace Dsa {
namespace Vehicle {

Vehicle::Vehicle(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_controller(new DsaController(this))
{
}

Vehicle::~Vehicle()
{
}

void Vehicle::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML SceneView component
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  connect(m_sceneView, &SceneQuickView::errorOccurred, m_controller, &DsaController::onError);

  // connect to the DSA controller errors
  connect(m_controller, &DsaController::errorOccurred, this, [this]
          (const QString& message, const QString& additionalMessage)
  {
    emit errorOccurred(message, additionalMessage);
  });

  m_controller->init(m_sceneView);

  // setup the connections from the view to the resource provider
  connect(m_sceneView, &SceneQuickView::spatialReferenceChanged,
          ToolResourceProvider::instance(), &ToolResourceProvider::spatialReferenceChanged);

  connect(m_sceneView, &SceneQuickView::mouseClicked,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMouseClicked);

  connect(m_sceneView, &SceneQuickView::mousePressed,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMousePressed);

  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMousePressedAndHeld);

  connect(m_sceneView, &SceneQuickView::mouseMoved,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMouseMoved);

  connect(m_sceneView, &SceneQuickView::mouseReleased,
          ToolResourceProvider::instance(), &ToolResourceProvider::onMouseReleased);

  connect(m_sceneView, &SceneQuickView::identifyGraphicsOverlayCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyGraphicsOverlayCompleted);

  connect(m_sceneView, &SceneQuickView::identifyGraphicsOverlaysCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyGraphicsOverlaysCompleted);

  connect(m_sceneView, &SceneQuickView::identifyLayerCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyLayerCompleted);

  connect(m_sceneView, &SceneQuickView::identifyLayersCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onIdentifyLayersCompleted);

  connect(m_sceneView, &SceneQuickView::screenToLocationCompleted,
          ToolResourceProvider::instance(), &ToolResourceProvider::onScreenToLocationCompleted);

  connect(ToolResourceProvider::instance(), &ToolResourceProvider::setMouseCursorRequested, this, [this](const QCursor& mouseCursor)
  {
    m_sceneView->setCursor(mouseCursor);
  });

  // Set scene to scene view
  m_sceneView->setArcGISScene(m_controller->scene());
}

} // Vehicle
} // Dsa
