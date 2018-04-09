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

#ifndef BASEMAPPICKERCONTROLLER_H
#define BASEMAPPICKERCONTROLLER_H


// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>
#include <QObject>

namespace Esri {
namespace ArcGISRuntime {
  class Basemap;
}
}

class QStringListModel;

namespace Dsa {

class TileCacheListModel;

class BasemapPickerController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* tileCacheModel READ tileCacheModel NOTIFY tileCacheModelChanged)

public:
  static const QString DEFAULT_BASEMAP_PROPERTYNAME;
  static const QString BASEMAP_DIRECTORY_PROPERTYNAME;

  explicit BasemapPickerController(QObject* parent = nullptr);
  ~BasemapPickerController();

  QAbstractListModel* tileCacheModel() const;

  Q_INVOKABLE void basemapSelected(int row);
  Q_INVOKABLE void selectInitialBasemap();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  QString basemapDataPath() const { return m_basemapDataPath; }
  void setBasemapDataPath(const QString& dataPath);
  QString defaultBasemap() const { return m_defaultBasemap; }
  void setDefaultBasemap(const QString& defaultBasemap);

public slots:
  void onBasemapDataPathChanged();

signals:
  void tileCacheModelChanged();
  void basemapsDataPathChanged();
  void basemapChanged(Esri::ArcGISRuntime::Basemap* basemap, QString name = "");
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

private:
  TileCacheListModel* m_tileCacheModel;
  int                 m_defaultBasemapIndex = 0;
  QString             m_basemapDataPath;
  QString             m_defaultBasemap = "topographic";

private:
  void findBasemaps();
};

} // Dsa

#endif // BASEMAPPICKERCONTROLLER_H
