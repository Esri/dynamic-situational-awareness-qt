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

#ifndef TILECACHE_LISTMODEL_H
#define TILECACHE_LISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QList>
#include <QMap>

namespace Esri::ArcGISRuntime {
  class TileCache;
  class VectorTileCache;
}

namespace Dsa {

  using TileCacheV = std::variant<std::monostate, Esri::ArcGISRuntime::TileCache*, Esri::ArcGISRuntime::VectorTileCache*>;
  struct VisitorData;
  struct VisitorTileCacheNameAt;

class TileCacheListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum TileCacheRoles
  {
    TileCacheTitleRole = Qt::UserRole + 1,
    TileCachePathRole = Qt::UserRole + 2,
    TileCacheThumbnailUrlRole = Qt::UserRole + 3
  };

  friend struct VisitorData;
  friend struct VisitorTileCacheNameAt;

  TileCacheListModel(QObject* parent = nullptr);
  ~TileCacheListModel();

  bool append(const QString& pathToTileCache);
  TileCacheV tileCacheAt(int row) const;
  QString tileCacheNameAt(int row) const;
  void clear();

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  template<typename T>
  TileCacheV getTileCacheFromPath(const QString& pathToTileCache);

  template<typename T>
  QVariant getData(TileCacheV tileCacheV, int role) const;

  template<typename T>
  QString getTileCacheNameAt(TileCacheV tileCacheV) const;

  QHash<int, QByteArray> m_roles;
  QList<TileCacheV> m_tileCacheData;
  QMap<QString, QUrl> m_thumbnailUrls;
};

} // Dsa

#endif // TILECACHE_LISTMODEL_H
