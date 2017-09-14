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

#ifndef DATAITEM_H
#define DATAITEM_H

#include <QObject>
#include <QFileInfo>
#include <QString>

class DataItem : public QObject
{
  Q_OBJECT

public:
  explicit DataItem(const QString& fullPath, QObject* parent = nullptr);
  ~DataItem() = default;

  Q_PROPERTY(QString fullPath READ fullPath CONSTANT)

  enum class DataType
  {
    Raster,
    Geodatabase,
    TilePackage,
    VectorTilePackage,
    MobileMapPackage,
    SceneLayerPackage,
    Shapefile,
    GeoPackage,
    KML,
    All,
    Unknown
  };

  QString fullPath() const { return m_fullPath; }
  QString fileName() const { return m_fileName; }
  DataType dataType() const { return m_dataType; }
  QString dataTypeAsString();

private:
  QString m_fullPath;
  QString m_fileName;
  DataType m_dataType;
  QFileInfo m_fileInfo;

  DataType determineDataType();
  void setFileName(const QString& fileName);
  void setDataType(const DataType& dataType);
};

#endif // DATAITEM_H

