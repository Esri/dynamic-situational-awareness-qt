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

// PCH header
#include "pch.hpp"

#include "Handheld.h"

// C++ API headers
#include "IdentifyGraphicsOverlayResult.h"
#include "IdentifyLayerResult.h"
#include "SceneQuickView.h"

// DSA headers
#include "DsaController.h"
#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {
namespace Handheld {

Handheld::Handheld(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent),
  m_controller(DsaController::instance())
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
  connect(m_sceneView, &SceneQuickView::errorOccurred, &m_controller, &DsaController::onError);

  // connect to the DSA controller errors
  connect(&m_controller, &DsaController::errorOccurred, this, [this](const QString& message, const QString& additionalMessage)
  {
    emit errorOccurred(message, additionalMessage);
  });

  auto* trp = ToolResourceProvider::instance();
  connect(trp, &ToolResourceProvider::sceneChanged, this, [this, trp]()
  {
    m_sceneView->setArcGISScene(trp->scene());
  });

  m_controller.init(m_sceneView);

  // setup the connections from the view to the resource provider
  connect(m_sceneView, &SceneQuickView::spatialReferenceChanged, trp, &ToolResourceProvider::spatialReferenceChanged);
  connect(m_sceneView, &SceneQuickView::mouseClicked,            trp, &ToolResourceProvider::onMouseClicked);
  connect(m_sceneView, &SceneQuickView::mousePressed,            trp, &ToolResourceProvider::onMousePressed);
  connect(m_sceneView, &SceneQuickView::mousePressedAndHeld,     trp, &ToolResourceProvider::onMousePressedAndHeld);
  connect(m_sceneView, &SceneQuickView::mouseMoved,              trp, &ToolResourceProvider::onMouseMoved);
  connect(m_sceneView, &SceneQuickView::mouseReleased,           trp, &ToolResourceProvider::onMouseReleased);
  connect(trp, &ToolResourceProvider::setMouseCursorRequested, this, [this](const QCursor& mouseCursor)
  {
    m_sceneView->setCursor(mouseCursor);
  });
}

void Handheld::resetToDefaultScene()
{
  DsaController::instance().resetToDefaultScene();
}

} // Handheld
} // Dsa

// Signal Documentation
/*!
  \fn void Handheld::errorOccurred(const QString& message, const QString& additionalMessage)
  \brief Signal emitted when an error occurs.

  An error \a message and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */
