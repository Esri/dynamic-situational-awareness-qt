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

// PCH header
#include "pch.hpp"

#include "DataItemListModel.h"

// Qt headers
#include <QAbstractListModel>
#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

namespace Dsa {

/*!
  \class DataItemListModel
  \inherits QAbstractListModel
  \brief A model responsible for storing data items (such as local layers) and reporting when they
  change.
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
DataItemListModel::DataItemListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

/*!
  \internal
 */
void DataItemListModel::setupRoles()
{
  m_roles[FullPathRole] = "fullPath";
  m_roles[FileNameRole] = "fileName";
}

/*!
  \brief Adds a new local data item located at \a fullPath.
 */
void DataItemListModel::addDataItem(const QString& fullPath)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_dataItems.append(fullPath);
  endInsertRows();
}

/*!
  \brief Returns the number of data items in the model.
 */
int DataItemListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_dataItems.count();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l DataItemRoles enum.
 */
QVariant DataItemListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_dataItems.count())
    return QVariant();

  DataItem dataItem = m_dataItems[index.row()];

  QVariant retVal;

  switch (role)
  {
  case FullPathRole:
    retVal = dataItem.fullPath;
    break;
  case FileNameRole:
    retVal = dataItem.fileName;
    break;
  default:
    break;
  }

  return retVal;
}

/*!
  \brief Returns the hash of role names used by the model.

  The roles are based on the \l DataItemRoles enum.
 */
QHash<int, QByteArray> DataItemListModel::roleNames() const
{
  return m_roles;
}

/*!
  \brief Clears the model.
 */
void DataItemListModel::clear()
{
  beginResetModel();
  m_dataItems.clear();
  endResetModel();
}

/*!
  \brief Returns the \l DataType of the item at \a index in the model.
 */
DataType DataItemListModel::getDataItemType(int index)
{
  if (index >= m_dataItems.size())
    return DataType::Unknown;

  return m_dataItems.at(index).dataType;
}

/*!
  \brief Returns the path of the item at \a index in the model.
 */
QString DataItemListModel::getDataItemPath(int index) const
{
  if (index >= m_dataItems.size())
    return QString();

  return m_dataItems.at(index).fullPath;
}

/*!
  \internal
  c'tor for DataItem struct
 */
DataItemListModel::DataItem::DataItem(const QString& fullPath):
  fullPath(fullPath)
{
  // set the file name
  QFileInfo fileInfo(fullPath);
  fileName = fileInfo.fileName();

  // determine the layer type
  QString fileExtension = fileInfo.completeSuffix();
  QStringList rasterExtensions{"img", "tif", "tiff", "i1", "dt0", "dt1", "dt2", "tc2", "geotiff", "hr1", "jpg", "jpeg", "jp2", "ntf", "png", "i21"};
  if (fileExtension == "geodatabase")
    dataType = DataType::Geodatabase;
  else if (fileExtension.compare("tpk", Qt::CaseInsensitive) == 0)
    dataType = DataType::TilePackage;
  else if (fileExtension.compare("shp", Qt::CaseInsensitive) == 0)
    dataType = DataType::Shapefile;
  else if (fileExtension.compare("gpkg", Qt::CaseInsensitive) == 0)
    dataType = DataType::GeoPackage;
  else if (fileExtension.compare("slpk", Qt::CaseInsensitive) == 0)
    dataType = DataType::SceneLayerPackage;
  else if (fileExtension.compare("vtpk", Qt::CaseInsensitive) == 0)
    dataType = DataType::VectorTilePackage;
  else if (fileExtension.compare("markup", Qt::CaseInsensitive) == 0)
    dataType = DataType::Markup;
  else if (rasterExtensions.contains(fileExtension.toLower()))
    dataType = DataType::Raster;
  else
    dataType = DataType::Unknown;
}

} // Dsa
