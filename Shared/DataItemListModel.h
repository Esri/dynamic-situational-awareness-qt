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

#ifndef DATAITEMLISTMODEL_H
#define DATAITEMLISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QByteArray>
#include <QFileInfo>
#include <QHash>
#include <QList>

namespace Dsa {

enum class DataType
{
  Raster,
  Geodatabase,
  TilePackage,
  VectorTilePackage,
  SceneLayerPackage,
  Shapefile,
  Markup,
  GeoPackage,
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

} // Dsa

#endif // DATAITEMLISTMODEL_H
