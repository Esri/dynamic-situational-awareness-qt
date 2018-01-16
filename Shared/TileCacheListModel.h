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

#ifndef TILECACHE_LISTMODEL_H
#define TILECACHE_LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QMap>

namespace Esri
{
namespace ArcGISRuntime
{
  class TileCache;
}
}

class TileCacheListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum TileCacheRoles
  {
    TileCacheTitleRole = Qt::UserRole + 1,
    TileCachePathRole = Qt::UserRole + 2,
    TileCacheThumbnaulUrlRole = Qt::UserRole + 3
  };

  TileCacheListModel(QObject* parent = nullptr);
  ~TileCacheListModel();

  bool append(const QString& pathToTileCache);
  Esri::ArcGISRuntime::TileCache* tileCacheAt(int row) const;
  QString tileCacheNameAt(int row) const;

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QHash<int, QByteArray>                  m_roles;
  QList<Esri::ArcGISRuntime::TileCache*>  m_tileCacheData;
  QMap<QString, QUrl>                     m_thumbnailUrls;
};

#endif // TILECACHE_LISTMODEL_H
