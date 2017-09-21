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

#ifndef ADDLOCALDATACONTROLLER_H
#define ADDLOCALDATACONTROLLER_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>

namespace Esri
{
namespace ArcGISRuntime
{
  class Layer;
  class ElevationSource;
}
}

class DataItemListModel;

class AddLocalDataController : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* localDataModel READ localDataModel NOTIFY localDataModelChanged)
  Q_PROPERTY(QStringList fileFilterList READ fileFilterList NOTIFY fileFilterListChanged)

public:
  explicit AddLocalDataController(QObject* parent = nullptr);
  ~AddLocalDataController() = default;

  Q_INVOKABLE void addPathToDirectoryList(const QString& path);
  Q_INVOKABLE void refreshLocalDataModel(const QString& fileType = "All");
  Q_INVOKABLE void addItemAsLayer(const QList<int>& index);
  Q_INVOKABLE void addItemAsElevationSource(const QList<int>& indices);
  QAbstractListModel* localDataModel() const;
  QString toolName() const;

  // helpers for creating the layers for a given string
  void createFeatureLayerGeodatabase(const QString& path);
  void createLayerGeoPackage(const QString& path);
  void createFeatureLayerShapefile(const QString& path);
  void createRasterLayer(const QString& path);
  void createKmlLayer(const QString& path);
  void createSceneLayer(const QString& path);
  void createTiledLayer(const QString& path);
  void createVectorTiledLayer(const QString& path);
  QStringList dataPaths() const { return m_dataPaths; }

signals:
  void localDataModelChanged();
  void layerSelected(Esri::ArcGISRuntime::Layer* layer);
  void elevationSourceSelected(Esri::ArcGISRuntime::ElevationSource* source);
  void fileFilterListChanged();

private:
  QStringList determineFileFilters(const QString& fileType);
  QStringList fileFilterList() const { return m_fileFilterList; }
  static const QString allData() { return s_allData; }
  static const QString rasterData() { return s_rasterData; }
  static const QString geodatabaseData() { return s_geodatabaseData; }
  static const QString shapefileData() { return s_shapefileData; }
  static const QString kmlData() { return s_kmlData; }
  static const QString geopackageData() { return s_geopackageData; }
  static const QString sceneLayerData() { return s_sceneLayerData; }
  static const QString vectorTilePackageData() { return s_vectorTilePackageData; }
  static const QString tilePackageData() { return s_tilePackageData; }

private:
  DataItemListModel* m_localDataModel;
  QStringList m_dataPaths;
  QStringList m_fileFilterList;
  static const QString s_allData;
  static const QString s_rasterData;
  static const QString s_geodatabaseData;
  static const QString s_shapefileData;
  static const QString s_kmlData;
  static const QString s_geopackageData;
  static const QString s_sceneLayerData;
  static const QString s_vectorTilePackageData;
  static const QString s_tilePackageData;
};

#endif // ADDLOCALDATACONTROLLER_H
