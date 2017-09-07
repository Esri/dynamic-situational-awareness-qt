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

//#include "BasemapListModel.h"
#include <QStringListModel>
#include <QDir>
#include <QDebug>

#include "ArcGISTiledLayer.h"
#include "Basemap.h"
#include "TileCache.h"

#include "DsaUtility.h"
#include "BasemapPickerController.h"

using namespace Esri::ArcGISRuntime;

BasemapPickerController::BasemapPickerController(QObject* parent /* = nullptr */):
  QObject(parent),
  m_basemapsModel(new QStringListModel(this))
{
  // placeholder until we have ToolManager
  DsaUtility::tools.append(this);

  m_basemapsDir = DsaUtility::dataPath();
  QDir basemapsDir (m_basemapsDir);
  emit basemapsDirChanged();

  basemapsDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  basemapsDir.setNameFilters(QStringList{"*.tpk"});

  QFileInfoList list = basemapsDir.entryInfoList();
  QStringList tpkNames;
  for (const QFileInfo& fInfo : list)
    tpkNames.append(fInfo.completeBaseName());

  m_basemapsModel->setStringList(tpkNames);
  emit basemapsModelChanged();
}

BasemapPickerController::~BasemapPickerController()
{
}

QAbstractListModel* BasemapPickerController::basemapsModel() const
{
  return m_basemapsModel;
}

void BasemapPickerController::basemapNameSelected(const QString &name)
{
  QFileInfo basemapFile = QFileInfo(QDir(m_basemapsDir), name + ".tpk");
  if (!basemapFile.exists())
    return;

  TileCache* tileCache = new TileCache(basemapFile.absoluteFilePath(), this);
  Basemap* selectedBasemap = new Basemap(new ArcGISTiledLayer(tileCache, this), this);

  emit basemapChanged(selectedBasemap);
}

QUrl BasemapPickerController::imageForBasemap(const QString &name)
{
  QFileInfo imageFile = QFileInfo(QDir(m_basemapsDir), name + ".png");
  if (!imageFile.exists())
    return QUrl();

  return QUrl::fromLocalFile(imageFile.filePath());
}

QString BasemapPickerController::toolName() const
{
  return QStringLiteral("basemap picker");
}
