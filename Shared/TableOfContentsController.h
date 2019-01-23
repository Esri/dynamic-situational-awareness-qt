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

#ifndef TABLEOFCONTENTSCONTROLLER_H
#define TABLEOFCONTENTSCONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractItemModel>
#include <QHash>

namespace Esri {
namespace ArcGISRuntime {
  class Layer;
  class LayerListModel;
}
}

namespace Dsa {

class DrawOrderLayerListModel;

class TableOfContentsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* layerListModel READ layerListModel NOTIFY layerListModelChanged)

public:

  enum class LayerGeometryType
  {
    Unknown = -1,
    Points = 0,
    Polylines = 1,
    Polygons = 2,
    Raster = 3,
    FreehandMarkup = 4,
    Kml = 5
  };
  Q_ENUM(LayerGeometryType)

  explicit TableOfContentsController(QObject* parent = nullptr);
  ~TableOfContentsController();

  QAbstractItemModel* layerListModel() const;

  Q_INVOKABLE void zoomTo(int layerIndex);
  Q_INVOKABLE void removeAt(int layerIndex);
  Q_INVOKABLE void moveUp(int layerIndex);
  Q_INVOKABLE void moveDown(int layerIndex);
  Q_INVOKABLE void moveFromTo(int fromIndex, int toIndex);

  Q_INVOKABLE QString alternateName(int layerIndex);
  Q_INVOKABLE LayerGeometryType layerGeometryType(int layerIndex);

  QString toolName() const override;

signals:
  void layerListModelChanged();

public slots:
  void updateLayerListModel();

private:
  int mappedIndex(int index) const;
  void refreshLayerOrder();

  Esri::ArcGISRuntime::LayerListModel* m_layerListModel = nullptr;
  QHash<Esri::ArcGISRuntime::Layer*, QMetaObject::Connection> m_layerConnections;
  DrawOrderLayerListModel* m_drawOrderModel = nullptr;
};

} // Dsa

#endif // TABLEOFCONTENTSCONTROLLER_H
