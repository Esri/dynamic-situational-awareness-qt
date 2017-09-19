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

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QList>

#include "DataItemListModel.h"

DataItemListModel::DataItemListModel(QObject* parent) :
  QAbstractListModel(parent)
{
  setupRoles();
}

void DataItemListModel::setupRoles()
{
  m_roles[FullPathRole] = "fullPath";
  m_roles[FileNameRole] = "fileName";
}

void DataItemListModel::addDataItem(const QString& fullPath)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_dataItems.append(fullPath);
  endInsertRows();
}

int DataItemListModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_dataItems.count();
}

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

QHash<int, QByteArray> DataItemListModel::roleNames() const
{
  return m_roles;
}

void DataItemListModel::clear()
{
  beginResetModel();
  m_dataItems.clear();
  endResetModel();
}

DataType DataItemListModel::getDataItemType(int index)
{
  return m_dataItems.at(index).dataType;
}

QString DataItemListModel::getDataItemPath(int index) const
{
  return m_dataItems.at(index).fullPath;
}

// c'tor for DataItem struct
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
  else if (fileExtension.compare("kml", Qt::CaseInsensitive) == 0)
    dataType = DataType::KML;
  else if (fileExtension.compare("slpk", Qt::CaseInsensitive) == 0)
    dataType = DataType::SceneLayerPackage;
  else if (fileExtension.compare("vtpk", Qt::CaseInsensitive) == 0)
    dataType = DataType::VectorTilePackage;
  else if (rasterExtensions.contains(fileExtension.toLower()))
    dataType = DataType::Raster;
  else
    dataType = DataType::Unknown;
}
