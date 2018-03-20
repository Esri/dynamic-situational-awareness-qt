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
#include "pch.hpp"

#include <QFileInfo>
#include <QTemporaryFile>
#include <QUrl>

#include "TileCache.h"

#include "TileCacheListModel.h"

using namespace Esri::ArcGISRuntime;

TileCacheListModel::TileCacheListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[TileCacheTitleRole] = "title";
  m_roles[TileCachePathRole] = "path";
  m_roles[TileCacheThumbnaulUrlRole] = "thumbnailUrl";
}

TileCacheListModel::~TileCacheListModel()
{

}

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

TileCache* TileCacheListModel::tileCacheAt(int row) const
{
  if (m_tileCacheData.size() <= row)
    return nullptr;

  return m_tileCacheData.at(row);
}

int TileCacheListModel::rowCount(const QModelIndex&) const
{
  return m_tileCacheData.size();
}

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

QHash<int, QByteArray> TileCacheListModel::roleNames() const
{
  return m_roles;
}

QString TileCacheListModel::tileCacheNameAt(int row) const
{
  if (m_tileCacheData.size() <= row)
    return "";

  return QFileInfo(m_tileCacheData.at(row)->path()).completeBaseName();
}

void TileCacheListModel::clear()
{
  beginResetModel();
  m_thumbnailUrls.clear();
  m_tileCacheData.clear();
  endResetModel();
}
