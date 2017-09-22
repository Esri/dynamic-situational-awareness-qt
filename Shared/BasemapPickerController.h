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

#ifndef BASEMAPPICKERCONTROLLER_H
#define BASEMAPPICKERCONTROLLER_H

#include <QObject>

#include <QAbstractListModel>
#include <QUrl>

#include "AbstractTool.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class Basemap;
}
}

class QStringListModel;

class TileCacheListModel;

class BasemapPickerController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* tileCacheModel READ tileCacheModel NOTIFY tileCacheModelChanged)

public:
  explicit BasemapPickerController(QObject* parent = nullptr);
  ~BasemapPickerController();

  QAbstractListModel* tileCacheModel() const;

  Q_INVOKABLE void basemapSelected(int row);
  Q_INVOKABLE void selectInitialBasemap();

  QString toolName() const override;

signals:
  void tileCacheModelChanged();
  void basemapsDirChanged();
  void basemapChanged(Esri::ArcGISRuntime::Basemap* basemap);

private:
  TileCacheListModel* m_tileCacheModel;
  int                 m_defaultBasemapIndex = 0;
};

#endif // BASEMAPPICKERCONTROLLER_H
