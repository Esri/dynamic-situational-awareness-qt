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

// PCH
#include "pch.hpp"

#include "TileCacheListModel.h"

// C++ API
#include "MapTypes.h"
#include "TileCache.h"
#include "VectorTileCache.h"
// Qt
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QUrl>
#include <QVariant>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

  struct VisitorData
  {
    VisitorData(const TileCacheListModel* owner, int role) :
      _this(owner),
      _role(role)
    {
    }

    const TileCacheListModel* _this;
    int _role;

    template<typename T>
    QVariant operator()(T arg)
    {
      return _this->getData<T>(arg, _role);
    }

    QVariant operator()(std::monostate)
    {
      return {};
    }
  };

  struct VisitorTileCacheNameAt
  {
    VisitorTileCacheNameAt(const TileCacheListModel* owner) :
      _this(owner)
    {
    }

    const TileCacheListModel* _this;

    template<typename T>
    QString operator()(T arg)
    {
      return _this->getTileCacheNameAt<T>(arg);
    }

    QString operator()(std::monostate)
    {
      return {};
    }
  };

/*!
  \class Dsa::TileCacheListModel
  \inmodule Dsa
  \inherits QAbstractListModel
  \brief A model for storing the list of
  \l Esri::ArcGISRuntime::TileCache files available for
  use as basemaps in the app.

  The model returns data for the following roles:
  \table
    \header
        \li Role
        \li Type
        \li Description
    \row
        \li title
        \li QString
        \li The title of the tile cache.
    \row
        \li fileName
        \li QString
        \li The file path to the tile cache.
    \row
        \li thumbnailUrl
        \li QUrl
        \li The URL to the thumbnail of the tile cache.
  \endtable
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
TileCacheListModel::TileCacheListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[TileCacheTitleRole] = "title";
  m_roles[TileCachePathRole] = "path";
  m_roles[TileCacheThumbnailUrlRole] = "thumbnailUrl";
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

  TileCacheVariant tileCacheV{};
  const int size = m_tileCacheData.size();
  if (pathToTileCache.endsWith(".tpk", Qt::CaseInsensitive))
  {
    tileCacheV = getTileCacheFromPath<TileCache>(pathToTileCache);
  }
  else if (pathToTileCache.endsWith(".vtpk", Qt::CaseInsensitive))
  {
    tileCacheV = getTileCacheFromPath<VectorTileCache>(pathToTileCache);
  }

  if (std::holds_alternative<std::monostate>(tileCacheV))
  {
    return false;
  }

  beginInsertRows(QModelIndex(), size, size);
  m_tileCacheData.append(tileCacheV);
  endInsertRows();

  return true;
}

/*!
  \brief Returns the tile cache at \a row in the list.
 */
TileCacheVariant TileCacheListModel::tileCacheAt(int row) const
{
  if (row < 0 || m_tileCacheData.size() <= static_cast<qsizetype>(row))
  {
    return {};
  }

  return m_tileCacheData.at(row);
}

/*!
  \brief Returns the number of tile caches in the model.

  /list
  /li /a parent - The parent object.
  /endlist

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
  {
    return {};
  }

  return std::visit(VisitorData{this, role}, m_tileCacheData.at(index.row()));
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
  if (m_tileCacheData.size() <= static_cast<qsizetype>(row))
  {
    return {};
  }

  return std::visit(VisitorTileCacheNameAt{this}, m_tileCacheData.at(row));
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

template<typename T>
TileCacheVariant TileCacheListModel::getTileCacheFromPath(const QString& pathToTileCache)
{
  auto* tileCache = new T(pathToTileCache, this);
  if (tileCache->path() != pathToTileCache)
  {
    delete tileCache;
    return {};
  }

  connect(tileCache, &T::loadStatusChanged, this, [this, tileCache](LoadStatus loadStatus)
  {
    if (loadStatus != LoadStatus::Loaded)
    {
      return;
    }

    QImage image = tileCache->thumbnail();
    if (image.isNull())
    {
      return;
    }

    QTemporaryFile* tempImgFile = new QTemporaryFile(QDir::temp().filePath(QStringLiteral("TileCacheXXXXXX.png")), tileCache);
    if (!tempImgFile->open())
    {
      return;
    }

    if (!image.save(tempImgFile->fileName()))
    {
      return;
    }

    m_thumbnailUrls.insert(tileCache->path(), QUrl::fromLocalFile(tempImgFile->fileName()));

    for (qsizetype i = 0; i < m_tileCacheData.size(); ++i)
    {
      const TileCacheVariant testCacheV = m_tileCacheData.at(i);
      if (!std::holds_alternative<T*>(testCacheV))
      {
        continue;
      }

      const auto* testCache = std::get<T*>(testCacheV);
      if (!testCache || testCache->path() != tileCache->path())
      {
        continue;
      }

      QModelIndex index = createIndex(i, 0);
      emit dataChanged(index, index);

      break;
    }
  });

  return tileCache;
}

template<typename T>
QVariant TileCacheListModel::getData(TileCacheVariant tileCacheV, int role) const
{
  auto* tileCache = std::get<T>(tileCacheV);
  if (!tileCache)
  {
    return {};
  }

  switch (role)
  {
    case TileCacheListModel::TileCacheTitleRole:
      return QFileInfo(tileCache->path()).completeBaseName();

    case TileCacheListModel::TileCachePathRole:
      return tileCache->path();

    case TileCacheListModel::TileCacheThumbnailUrlRole:
    {
      if (tileCache->loadStatus() == LoadStatus::NotLoaded)
      {
        tileCache->load();
      }

      return m_thumbnailUrls.value(tileCache->path(), QUrl());
    }
    default:
      break;
  }

  return {};
}

template<typename T>
QString TileCacheListModel::getTileCacheNameAt(TileCacheVariant tileCacheV) const
{
  return getData<T>(tileCacheV, TileCacheListModel::TileCacheTitleRole).toString();
}

} // Dsa
