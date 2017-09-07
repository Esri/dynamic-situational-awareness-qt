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

namespace Esri
{
namespace ArcGISRuntime
{
  class Basemap;
}
}

class QStringListModel;

class BasemapPickerController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* basemapsModel READ basemapsModel NOTIFY basemapsModelChanged)

public:
  BasemapPickerController(QObject* parent = nullptr);
  ~BasemapPickerController();

  QAbstractListModel* basemapsModel() const;

  Q_INVOKABLE void basemapNameSelected(const QString& name);
  Q_INVOKABLE QUrl imageForBasemap(const QString& name);

  QString toolName() const;

signals:
  void basemapsModelChanged();
  void basemapsDirChanged();
  void basemapChanged(Esri::ArcGISRuntime::Basemap* basemap);

private:
  QStringListModel* m_basemapsModel;
  QString           m_basemapsDir;
};

#endif // BASEMAPPICKERCONTROLLER_H
