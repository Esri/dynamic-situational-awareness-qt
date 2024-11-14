/*******************************************************************************
 *  Copyright 2012-2024 Esri
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

#include "PackagesController.h"

// #include "AbstractTool.h"

namespace Dsa {

PackagesController::PackagesController(QObject* parent /* = nullptr */):
  AbstractTool(parent)
{
  // connect(this, &BasemapPickerController::basemapsDataPathChanged, this, &BasemapPickerController::onBasemapDataPathChanged);

  ToolManager::instance().addTool(this);
}

QString PackagesController::toolName() const
{
  return QStringLiteral("packages");
}

void PackagesController::setProperties(const QVariantMap& properties)
{
  Q_UNUSED(properties);
}

} // namespace Dsa
