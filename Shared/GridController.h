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
#include <QtContainerFwd>

// STL headers
#include <unordered_map>

namespace Esri::ArcGISRuntime {
  class Grid;
}

namespace Dsa {

class GridController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool gridVisible READ gridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
  Q_PROPERTY(QString gridColorScheme READ gridColorScheme WRITE setGridColorScheme NOTIFY gridColorSchemeChanged)
  Q_PROPERTY(int gridColorSchemeIndex READ gridColorSchemeIndex NOTIFY gridColorSchemeIndexChanged)
  Q_PROPERTY(QString coordinateFormat READ coordinateFormat WRITE setCoordinateFormat NOTIFY coordinateFormatChanged)
  Q_PROPERTY(QStringList gridColorSchemes READ gridColorSchemes NOTIFY gridColorSchemesChanged)

public:
  inline static const QString PROPERTY_NAME_GRID_VISIBLE = QStringLiteral("GridVisible");
  inline static const QString GRID_VISIBLE_VALUE_DEFAULT = QStringLiteral("false");
  inline static const QString PROPERTY_NAME_GRID_COLOR_SCHEME = QStringLiteral("GridColorScheme");
  inline static const QString GRID_COLOR_SCHEME_VALUE_DARK = QStringLiteral("Dark");
  inline static const QString GRID_COLOR_SCHEME_VALUE_LIGHT = QStringLiteral("Light");
  inline static const QString GRID_COLOR_SCHEME_VALUE_COLORS = QStringLiteral("Colors");
  inline static const QString GRID_COLOR_SCHEME_VALUE_DEFAULT = GRID_COLOR_SCHEME_VALUE_DARK;

  explicit GridController(QObject* parent = nullptr);

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;
  void setProperty(const QString& propertyName, const QVariantMap& properties) override;

  bool gridVisible() const;
  void setGridVisible(bool gridVisible);
  QString gridColorScheme();
  void setGridColorScheme(const QString& gridColorScheme);
  int gridColorSchemeIndex() const;
  QString coordinateFormat() const;
  void setCoordinateFormat(const QString& coordinateFormat);
  QStringList gridColorSchemes() const;

signals:
  void gridVisibleChanged();
  void gridColorSchemeChanged();
  void coordinateFormatChanged();
  void gridColorSchemeIndexChanged();
  void gridColorSchemesChanged();

private:
  bool m_gridVisible;
  QString m_gridColorScheme;
  int m_gridColorSchemeIndex;
  QString m_coordinateFormat;
  QStringList m_gridColorSchemes;

  Esri::ArcGISRuntime::Grid* m_grid = nullptr;
  std::unordered_map<QString, Esri::ArcGISRuntime::Grid*> m_grids;
};

}

#endif // GRIDCONTROLLER_H
