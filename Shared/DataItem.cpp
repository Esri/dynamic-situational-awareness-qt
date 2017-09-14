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

#include "Layer.h"

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "DataItem.h"

using namespace Esri::ArcGISRuntime;

DataItem::DataItem(const QString& fullPath, QObject* parent /*= nullptr*/) :
  QObject(parent),
  m_fullPath(fullPath),
  m_dataType(DataType::Unknown),
  m_fileInfo(QFileInfo(fullPath))
{
  // set the file name
  setFileName(m_fileInfo.fileName());
  // determine and set the layer type
  setDataType(determineDataType());
}

DataItem::DataType DataItem::determineDataType()
{
  QString fileExtension = m_fileInfo.completeSuffix();
  QStringList rasterExtensions{"img", "tif", "tiff", "I1", "dt0", "dt1", "dt2", "tc2", "geotiff", "hr1", "jpg", "jpeg", "jp2", "ntf", "png", "i21"};
  if (fileExtension == "geodatabase")
    return DataType::Geodatabase;
  else if (fileExtension.toLower() == "tpk")
    return DataType::TilePackage;
  else if (fileExtension.toLower() == "shp")
    return DataType::Shapefile;
  else if (fileExtension.toLower() == "mmpk")
    return DataType::MobileMapPackage;
  else if (fileExtension.toLower() == "gpkg")
    return DataType::GeoPackage;
  else if (fileExtension.toLower() == "kml")
    return DataType::KML;
  else if (fileExtension.toLower() == "slpk")
    return DataType::SceneLayerPackage;
  else if (fileExtension.toLower() == "vtpk")
    return DataType::VectorTilePackage;
  else if (rasterExtensions.contains(fileExtension.toLower()))
    return DataType::Raster;
  else
    return DataType::Unknown;
}

void DataItem::setFileName(const QString& fileName)
{
  m_fileName = fileName;
}

void DataItem::setDataType(const DataType& dataType)
{
  m_dataType = dataType;
}

QString DataItem::dataTypeAsString()
{
  switch (dataType()) {
  case DataType::Geodatabase:
    return "Geodatabase";
    break;
  case DataType::GeoPackage:
    return "GeoPackage";
    break;
  case DataType::KML:
    return "KML";
    break;
  case DataType::MobileMapPackage:
    return "Mobile Map Package";
    break;
  case DataType::Raster:
    return "Raster";
    break;
  case DataType::SceneLayerPackage:
    return "Scene Layer Package";
    break;
  case DataType::Shapefile:
    return "Shapefile";
    break;
  case DataType::TilePackage:
    return "Tile Package";
    break;
  case DataType::VectorTilePackage:
    return "Vector Tile Package";
    break;
  default:
    return "Unknown Layer";
    break;
  }
}
