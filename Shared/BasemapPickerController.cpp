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

#include <QDir>

#include "ArcGISTiledLayer.h"
#include "Basemap.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "DsaUtility.h"
#include "BasemapPickerController.h"
#include "TileCacheListModel.h"

using namespace Esri::ArcGISRuntime;

BasemapPickerController::BasemapPickerController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_tileCacheModel(new TileCacheListModel(this))
{
  Toolkit::ToolManager::instance()->addTool(this);

  QString basemapsPath = DsaUtility::dataPath();
  QDir basemapsDir(basemapsPath);
  emit basemapsDirChanged();

  basemapsDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  basemapsDir.setNameFilters(QStringList{"*.tpk"});

  QFileInfoList list = basemapsDir.entryInfoList();
  int index = -1;
  for (const QFileInfo& fInfo : list)
  {
    if (m_tileCacheModel->append(fInfo.filePath()))
      index++;

    if(fInfo.completeBaseName().compare("topographic", Qt::CaseInsensitive) == 0)
      m_defaultBasemapIndex = index;
  }

  emit tileCacheModelChanged();
}

BasemapPickerController::~BasemapPickerController()
{
}

QAbstractListModel* BasemapPickerController::tileCacheModel() const
{
  return m_tileCacheModel;
}

void BasemapPickerController::basemapSelected(int row)
{
  TileCache* tileCache = m_tileCacheModel->tileCacheAt(row);
  if (!tileCache)
    return;

  Basemap* selectedBasemap = new Basemap(new ArcGISTiledLayer(tileCache, this), this);
  connect(selectedBasemap, &Basemap::errorOccurred, this, &BasemapPickerController::errorOccurred);

  Toolkit::ToolManager::instance()->resourceProvider()->setBasemap(selectedBasemap);

  emit basemapChanged(selectedBasemap);
}

void BasemapPickerController::selectInitialBasemap()
{
  basemapSelected(m_defaultBasemapIndex);
}

QString BasemapPickerController::toolName() const
{
  return QStringLiteral("basemap picker");
}
