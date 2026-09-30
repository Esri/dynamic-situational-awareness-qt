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

#ifndef BASEMAPPICKERCONTROLLER_H
#define BASEMAPPICKERCONTROLLER_H

// DSA
#include "AbstractTool.h"
// Qt
#include <QObject>
// Std
#include <variant>

class QAbstractListModel;

namespace Esri::ArcGISRuntime {
  class Basemap;
  class TileCache;
  class VectorTileCache;
}

class QStringListModel;

namespace Dsa {

  using TileCacheV = std::variant<std::monostate, Esri::ArcGISRuntime::TileCache*, Esri::ArcGISRuntime::VectorTileCache*>;
  struct VisitorSelectBasemap;

class TileCacheListModel;
Q_MOC_INCLUDE("TileCacheListModel.h")

class BasemapPickerController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* tileCacheModel READ tileCacheModel NOTIFY tileCacheModelChanged)
  Q_PROPERTY(int selectedBasemapIndex READ selectedBasemapIndex NOTIFY selectedBasemapIndexChanged)
  Q_PROPERTY(QString selectedBasemapPath READ selectedBasemapPath NOTIFY selectedBasemapIndexChanged)

  friend struct VisitorSelectBasemap;

public:
  static const QString DEFAULT_BASEMAP_PROPERTYNAME;
  static const QString BASEMAP_DIRECTORY_PROPERTYNAME;

  explicit BasemapPickerController(QObject* parent = nullptr);
  ~BasemapPickerController();

  QAbstractListModel* tileCacheModel() const;

  Q_INVOKABLE void basemapSelected(int row);
  Q_INVOKABLE void selectInitialBasemap();

  QString toolName() const override;
  void toolInitProperties(const QVariantMap& properties) override;
  bool shouldSetProperties(const QString& propertyName) override;

  QString basemapDataPath() const;
  void setBasemapDataPath(const QString& dataPath);
  QString defaultBasemap() const;
  void setDefaultBasemap(const QString& defaultBasemap);
  int selectedBasemapIndex() const;
  QString selectedBasemapPath() const;

public slots:
  void onBasemapDataPathChanged();

signals:
  void tileCacheModelChanged();
  void basemapsDataPathChanged();
  void basemapChanged(Esri::ArcGISRuntime::Basemap* basemap, QString name = "");
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  void selectedBasemapIndexChanged();

private:

  template<typename T, typename L>
  void selectBasemap(TileCacheV tileCacheV, int row);

  TileCacheListModel* m_tileCacheModel;
  int                 m_defaultBasemapIndex = 0;
  QString             m_basemapDataPath;
  QString             m_defaultBasemap = "topographic";
  int                 m_selectedBasemapIndex = -1;
  QString             m_selectedBasemapPath;

private:
  void findBasemaps();
};

} // Dsa

#endif // BASEMAPPICKERCONTROLLER_H
