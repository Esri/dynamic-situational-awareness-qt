// Copyright 2016 ESRI
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

#include <QObject>

#include <QAbstractListModel>
#include <QHash>
#include <QUrl>

#include "AbstractTool.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class Layer;
  class LayerListModel;
}
}

class TableOfContentsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* layerListModel READ layerListModel NOTIFY layerListModelChanged)

public:

  enum class LayerGeometryType
  {
    Unknown = -1,
    Points = 0,
    Polylines = 1,
    Polygons = 2,
    Raster = 3
  };
  Q_ENUM(LayerGeometryType)

  explicit TableOfContentsController(QObject* parent = nullptr);
  ~TableOfContentsController();

  QAbstractListModel* layerListModel() const;

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

private slots:
  void updateLayerListModel();

private:
  Esri::ArcGISRuntime::LayerListModel* m_layerListModel = nullptr;
  QHash<Esri::ArcGISRuntime::Layer*, QMetaObject::Connection> m_layerConnections;
};

#endif // TABLEOFCONTENTSCONTROLLER_H
