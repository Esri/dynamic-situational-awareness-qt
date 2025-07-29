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

// PCH header
#include "pch.hpp"

#include "GridController.h"

// DSA headers
#include "NavigationController.h"

// Toolkit headers
#include "CoordinateConversionConstants.h"

// C++ API headers
#include "GeoView.h"
#include "LatitudeLongitudeGrid.h"
#include "MapViewTypes.h"
#include "MGRSGrid.h"
#include "USNGGrid.h"
#include "UTMGrid.h"

// STL headers
#include <algorithm>
#include <vector>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

GridController::GridController(QObject* parent):
  AbstractTool(parent)
{
  m_conToolAdded = connect(&ToolManager::instance(), &ToolManager::toolAdded, this, &GridController::onToolAdded);

  ToolManager::instance().addTool(this);
}

QString GridController::toolName() const
{
  return "GridController";
}

void GridController::onToolAdded(AbstractTool* newTool)
{
  // find the navigation controller
  Q_UNUSED(newTool);
  const auto* navigationController = ToolManager::instance().tool<NavigationController>();
  if (!navigationController)
    return;

  // create a connection to the signal for when an elevation source is selected/changed
  connect(navigationController, &NavigationController::showGridChanged, this, [this](bool show)
  {
    setShowGrid(show);
  });

  // stop listening for new tools being added
  disconnect(m_conToolAdded);
}

void GridController::setProperties(const QVariantMap &properties)
{
  // create a string lookup to use lat long for everything but utm/mgrs/usng
  using namespace Esri::ArcGISRuntime::Toolkit;
  const static std::vector<QString> nonDefaultFormatNames{
    CoordinateConversionConstants::UTM_FORMAT,
    CoordinateConversionConstants::MGRS_FORMAT,
    CoordinateConversionConstants::USNG_FORMAT
  };

  auto* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  // access tool properties from the config
  auto formatLookup = QString{QStringLiteral("default")};
  auto coordinateFormat = properties["CoordinateFormat"].toString();
  if (std::find(nonDefaultFormatNames.cbegin(), nonDefaultFormatNames.cend(), coordinateFormat) != nonDefaultFormatNames.cend())
    formatLookup = coordinateFormat;

  // create an instance of the matching grid type if it has not been selected yet
  if (m_grids.count(formatLookup) < 1)
  {
    if (formatLookup == CoordinateConversionConstants::UTM_FORMAT)
      m_grids[formatLookup] = new UTMGrid(this);
    else if (formatLookup == CoordinateConversionConstants::MGRS_FORMAT)
      m_grids[formatLookup] = new MGRSGrid(this);
    else if (formatLookup == CoordinateConversionConstants::USNG_FORMAT)
      m_grids[formatLookup] = new USNGGrid(this);
    else
    {
      m_grids[formatLookup] = new LatitudeLongitudeGrid(this);
      m_grids[formatLookup]->setLabelPosition(GridLabelPosition::Geographic);
    }
  }

  m_grid = m_grids[formatLookup];
  if (!m_grid)
    return;

  geoView->setGrid(m_grid);
  m_grid->setVisible(m_showGrid);
}

void GridController::setShowGrid(bool show)
{
  m_showGrid = show;
  if (m_grid)
    m_grid->setVisible(m_showGrid);
}

}
