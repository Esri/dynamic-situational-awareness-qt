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
#include <QUrl>

#include "AbstractTool.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class LayerListModel;
}
}

class TableOfContentsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* layerListModel READ layerListModel NOTIFY layerListModelChanged)

public:
  explicit TableOfContentsController(QObject* parent = nullptr);
  ~TableOfContentsController();

  QAbstractListModel* layerListModel() const;

  Q_INVOKABLE void zoomTo(int layerIndex);
  Q_INVOKABLE void removeAt(int layerIndex);
  Q_INVOKABLE void moveUp(int layerIndex);
  Q_INVOKABLE void moveDown(int layerIndex);

  Q_INVOKABLE QString getAlternateName(int layerIndex);

  QString toolName() const override;

signals:
  void layerListModelChanged();

private slots:
  void updateLayerListModel();

private:
  Esri::ArcGISRuntime::LayerListModel* m_layerListModel;
};

#endif // TABLEOFCONTENTSCONTROLLER_H
