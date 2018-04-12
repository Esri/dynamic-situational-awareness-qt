// Copyright 2018 ESRI
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

// PCH header
#include "pch.hpp"

#include "TileCacheListModel.h"

// C++ API headers
#include "TileCache.h"

// Qt headers
#include <QFileInfo>
#include <QTemporaryFile>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class TileCacheListModel
  \inherits QAbstractListModel
  \brief A model for storing the list of
  \l Esri::ArcGISRuntime::TileCache files available for
  use as basemaps in the app.
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
TileCacheListModel::TileCacheListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[TileCacheTitleRole] = "title";
  m_roles[TileCachePathRole] = "path";
  m_roles[TileCacheThumbnaulUrlRole] = "thumbnailUrl";
}

/*!
  \brief Destructor.
 */
TileCacheListModel::~TileCacheListModel()
{

}

/*!
  \brief Adds the tile cache at \a pathToTileCache to the list.

  Returns whether the file was successfully added.
 */
bool TileCacheListModel::append(const QString& pathToTileCache)
{
  QFileInfo fileInfo(pathToTileCache);
  if (!fileInfo.exists())
    return false;

  TileCache* tileCache = new TileCache(pathToTileCache, this);
  if (tileCache->path() != pathToTileCache)
  {
    delete tileCache;
    return false;
  }

  const int size = m_tileCacheData.size();

  connect(tileCache, &TileCache::loadStatusChanged, this, [this, tileCache](LoadStatus loadStatus)
  {
    if (loadStatus != LoadStatus::Loaded)
      return;

    QImage image = tileCache->thumbnail();
    if (image.isNull())
      return;

    QTemporaryFile* tempImgFile = new QTemporaryFile("TileCacheXXXXXX.png", tileCache);
    if (!tempImgFile->open())
      return;

    if (!image.save(tempImgFile->fileName()))
      return;

    m_thumbnailUrls.insert(tileCache->path(), QUrl::fromLocalFile(tempImgFile->fileName()));

    for (int i = 0; i < m_tileCacheData.size(); ++i)
    {
      const TileCache* testCache = m_tileCacheData.at(i);
      if (!testCache || testCache->path() != tileCache->path())
        continue;

      QModelIndex index = createIndex(i, 0);
      emit dataChanged(index, index);

      break;
    }
  });


  beginInsertRows(QModelIndex(), size, size);
  m_tileCacheData.append(tileCache);
  endInsertRows();

  return true;
}

/*!
  \brief Returns the tile cache at \a row in the list.
 */
TileCache* TileCacheListModel::tileCacheAt(int row) const
{
  if (m_tileCacheData.size() <= row)
    return nullptr;

  return m_tileCacheData.at(row);
}

/*!
  \brief Returns the number of tile caches in the model.
 */
int TileCacheListModel::rowCount(const QModelIndex&) const
{
  return m_tileCacheData.size();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l TileCacheRoles enum.
 */
QVariant TileCacheListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return QVariant();

  TileCache* tileCache = m_tileCacheData.at(index.row());
  if (!tileCache)
    return QVariant();

  switch (role)
  {
  case TileCacheTitleRole:
    return QFileInfo(tileCache->path()).completeBaseName();
    break;
  case TileCachePathRole:
    return tileCache->path();
    break;
  case TileCacheThumbnaulUrlRole:
  {
    if (tileCache->loadStatus() == LoadStatus::NotLoaded)
      tileCache->load();

    return m_thumbnailUrls.value(tileCache->path(), QUrl());
  }
  default:
    break;
  }

  return QVariant();
}

/*!
  \brief Returns the hash of role names used by the model.

  The roles are based on the \l DataItemRoles enum.
 */
QHash<int, QByteArray> TileCacheListModel::roleNames() const
{
  return m_roles;
}

/*!
  \brief Returns the name of the tile cache at \a row in the list.
 */
QString TileCacheListModel::tileCacheNameAt(int row) const
{
  if (m_tileCacheData.size() <= row)
    return "";

  return QFileInfo(m_tileCacheData.at(row)->path()).completeBaseName();
}

/*!
  \brief Clears the model.
 */
void TileCacheListModel::clear()
{
  beginResetModel();
  m_thumbnailUrls.clear();
  m_tileCacheData.clear();
  endResetModel();
}

} // Dsa
