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
    FreehandMarkup = 4
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

  Esri::ArcGISRuntime::LayerListModel* m_layerListModel = nullptr;
  QHash<Esri::ArcGISRuntime::Layer*, QMetaObject::Connection> m_layerConnections;
  DrawOrderLayerListModel* m_drawOrderModel = nullptr;
};

} // Dsa

#endif // TABLEOFCONTENTSCONTROLLER_H
