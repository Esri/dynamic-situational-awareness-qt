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

#ifndef DATAITEMLISTMODEL_H
#define DATAITEMLISTMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QFileInfo>

enum class DataType
{
  Raster,
  Geodatabase,
  TilePackage,
  VectorTilePackage,
  SceneLayerPackage,
  Shapefile,
  GeoPackage,
  KML,
  All,
  Unknown = -1
};

class DataItemListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum DataItemRoles
  {
    FullPathRole = Qt::UserRole + 1,
    FileNameRole
  };

  explicit DataItemListModel(QObject* parent = nullptr);
  ~DataItemListModel() = default;

  DataType getDataItemType(int index);
  QString getDataItemPath(int index) const;
  void addDataItem(const QString& fullPath);
  void clear();
  void setupRoles();
  int size() { return m_dataItems.size(); }

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  // DataItem struct to keep track of the name, path, and type
  struct DataItem
  {
  public:
    DataItem(const QString& fullPath);
    ~DataItem() = default;

    QString fullPath;
    QString fileName;
    DataType dataType;
  };

  QHash<int, QByteArray> m_roles;
  QList<DataItem> m_dataItems;
};

#endif // DATAITEMLISTMODEL_H
