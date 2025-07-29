/*******************************************************************************
 *  Copyright 2012-2025 Esri
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

#ifndef GRIDCONTROLLER_H
#define GRIDCONTROLLER_H

// DSA headers
#include "AbstractTool.h"

// Qt headers
#include <QString>
#include <QVariantMap>

// STL headers
#include <unordered_map>

namespace Esri::ArcGISRuntime {
  class Grid;
}

namespace Dsa {

class GridController : public AbstractTool
{
  Q_OBJECT

public:
  explicit GridController(QObject* parent = nullptr);

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  void setShowGrid(bool show);

private:
  void onToolAdded(AbstractTool* newTool);

  bool m_showGrid = false;
  QMetaObject::Connection m_conToolAdded;
  Esri::ArcGISRuntime::Grid* m_grid = nullptr;
  std::unordered_map<QString, Esri::ArcGISRuntime::Grid*> m_grids;
};

}

#endif // GRIDCONTROLLER_H
