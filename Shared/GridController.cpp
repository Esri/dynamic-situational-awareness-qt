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

// Toolkit headers
#include "CoordinateConversionConstants.h"

// C++ API headers
#include "GeoView.h"
#include "LatitudeLongitudeGrid.h"
#include "MapViewTypes.h"
#include "MGRSGrid.h"
#include "SimpleLineSymbol.h"
#include "USNGGrid.h"
#include "UTMGrid.h"

// Qt headers
#include <QVariantMap>

// STL headers
#include <algorithm>
#include <unordered_set>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

GridController::GridController(QObject* parent):
  AbstractTool(parent),
  m_gridVisible(false),
  m_gridColorScheme(GRID_COLOR_SCHEME_VALUE_DEFAULT),
  m_gridColorSchemes{GRID_COLOR_SCHEME_VALUE_DARK,
                     GRID_COLOR_SCHEME_VALUE_LIGHT,
                     GRID_COLOR_SCHEME_VALUE_COLORS}
{
  ToolManager::instance().addTool(this);
}

QString GridController::toolName() const
{
  return "GridController";
}

void GridController::setProperties(const QVariantMap& properties)
{
  // set the member variables to the values from the config file initially
  static bool initialized = false;
  if (initialized)
    return;
  initialized = true;

  // set the private
  if (const auto gridVisibleVar = properties[PROPERTY_NAME_GRID_VISIBLE]; gridVisibleVar.canConvert<bool>())
    setGridVisible(gridVisibleVar.toBool());

  // make sure the color scheme is one of the available strings in the list
  const auto gridColorSchemeVal = properties[PROPERTY_NAME_GRID_COLOR_SCHEME].toString();
  for (const auto& gridColorSchemeName : std::as_const(m_gridColorSchemes))
  {
    if (gridColorSchemeName == gridColorSchemeVal)
    {
      setGridColorScheme(gridColorSchemeVal);
      break;
    }
  }

  // always set the coordinate format. the available values come from the toolkit
  // control list and there is a default setting in the controller logic
  setCoordinateFormat(properties["CoordinateFormat"].toString());
}

bool GridController::gridVisible() const
{
  return m_gridVisible;
}

void GridController::setGridVisible(bool gridVisible)
{
  m_gridVisible = gridVisible;
  if (m_grid)
    m_grid->setVisible(m_gridVisible);

  emit gridVisibleChanged();
  emit propertyChanged(PROPERTY_NAME_GRID_VISIBLE, m_gridVisible);
}

QString GridController::gridColorScheme()
{
  return m_gridColorScheme;
}

void GridController::setGridColorScheme(const QString& gridColorScheme)
{
  m_gridColorScheme = gridColorScheme;
  m_gridColorSchemeIndex = m_gridColorSchemes.indexOf(m_gridColorScheme);

  const static QHash<QString, QColor> gridLineColors
  {
    { GRID_COLOR_SCHEME_VALUE_DARK, QColorConstants::DarkGray },
    { GRID_COLOR_SCHEME_VALUE_LIGHT, QColorConstants::LightGray },
    { GRID_COLOR_SCHEME_VALUE_COLORS, QColorConstants::Blue }
  };

  if (m_grid)
  {
    if (auto* sls = static_cast<SimpleLineSymbol*>(m_grid->lineSymbol(0)); sls)
    {
      sls->setColor(gridLineColors[m_gridColorScheme]);
      for (qsizetype i = 0; i < m_grid->levelCount(); i++)
        m_grid->setLineSymbol(i, sls);
    }
  }
  emit gridColorSchemeChanged();
  emit gridColorSchemeIndexChanged();
  emit propertyChanged(PROPERTY_NAME_GRID_COLOR_SCHEME, m_gridColorScheme);
}

QString GridController::coordinateFormat() const
{
  return m_coordinateFormat;
}

void GridController::setCoordinateFormat(const QString& coordinateFormat)
{
  m_coordinateFormat = coordinateFormat;

  // create a string lookup to use lat long for everything but utm/mgrs/usng
  using namespace Esri::ArcGISRuntime::Toolkit;
  static const std::unordered_set<QString> nonDefaultFormatNames{
    CoordinateConversionConstants::UTM_FORMAT,
    CoordinateConversionConstants::MGRS_FORMAT,
    CoordinateConversionConstants::USNG_FORMAT,
    CoordinateConversionConstants::DECIMAL_DEGREES_FORMAT,
    CoordinateConversionConstants::DEGREES_MINUTES_SECONDS_FORMAT
  };

  auto* geoView = ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  // access tool properties from the config
  auto formatLookup = QString{QStringLiteral("default")};
  if (std::find(std::cbegin(nonDefaultFormatNames), std::cend(nonDefaultFormatNames), coordinateFormat) != std::cend(nonDefaultFormatNames))
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

  // apply the current state of the controller to the active grid
  geoView->setGrid(m_grid);
  setGridVisible(m_gridVisible);
  setGridColorScheme(m_gridColorScheme);

  if (m_grid->gridType() != GridType::LatitudeLongitudeGrid)
    return;

  // adjust the format string for lat/long style grids
  auto* latLongGrid = static_cast<LatitudeLongitudeGrid*>(m_grid);
  if (formatLookup == CoordinateConversionConstants::DEGREES_MINUTES_SECONDS_FORMAT)
    latLongGrid->setLabelFormat(LatitudeLongitudeGridLabelFormat::DegreesMinutesSeconds);
  else
    latLongGrid->setLabelFormat(LatitudeLongitudeGridLabelFormat::DecimalDegrees);
}

int GridController::gridColorSchemeIndex() const
{
  return m_gridColorSchemeIndex;
}

QStringList GridController::gridColorSchemes() const
{
  return m_gridColorSchemes;
}

}
