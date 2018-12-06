
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

#include "PackageImageProvider.h"
#include "OpenMobileScenePackageController.h"

// toolkit headers
#include "ToolManager.h"

using namespace Esri::ArcGISRuntime::Toolkit;

namespace Dsa {

/*!
  \class Dsa::PackageImageProvider
  \inmodule Dsa
  \inherits QQuickImageProvider
  \brief Image provider for the thumbnails contained in mobile scene packages.

  \sa Dsa::OpenMobileScenePackageController
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
PackageImageProvider::PackageImageProvider(QObject* parent /*= nullptr*/) :
  QQuickImageProvider(QQuickImageProvider::Image),
  QObject(parent),
  m_defaultImage(":/Resources/AppIcon.png")
{
  // Find the OpenMobileScenePackageController and connect to the imageReady signal
  m_findToolConnection = connect(&ToolManager::instance(), &ToolManager::toolAdded, [this](AbstractTool* newTool)
  {
    if (!newTool)
      return;

    auto candidateTool = qobject_cast<OpenMobileScenePackageController*>(newTool);
    if (!candidateTool)
      return;

    m_packageController = candidateTool;

    // store images created by the tool
    connect(m_packageController, &OpenMobileScenePackageController::imageReady, this, [this](const QString& packageName, const QImage& packageImage)
    {
    m_packages.insert(packageName, packageImage.copy());
    });

  disconnect(m_findToolConnection);
});
}

/*!
  \brief Return the image with the specified \a id.

  If none is foound, return a default image.
 */
QImage PackageImageProvider::requestImage(const QString &id, QSize* /*size*/, const QSize& /*requestedSize*/)
{
  auto findIt = m_packages.constFind(id);
  auto image = findIt == m_packages.constEnd() ? m_defaultImage : findIt.value();
  return image.isNull() ? m_defaultImage : image;
}

} // Dsa
