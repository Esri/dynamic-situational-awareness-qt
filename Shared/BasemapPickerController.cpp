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

#include "BasemapPickerController.h"
#include "TileCacheListModel.h"

using namespace Esri::ArcGISRuntime;

const QString BasemapPickerController::DEFAULT_BASEMAP_PROPERTYNAME = "DefaultBasemap";
const QString BasemapPickerController::BASEMAP_DIRECTORY_PROPERTYNAME = "BasemapDirectory";

// Default c'tor
// User must call setBasemapDataPath to populate the basemap list
BasemapPickerController::BasemapPickerController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_tileCacheModel(new TileCacheListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(this, &BasemapPickerController::basemapsDataPathChanged, this, &BasemapPickerController::onBasemapDataPathChanged);
}

BasemapPickerController::~BasemapPickerController()
{
}

void BasemapPickerController::setBasemapDataPath(const QString& dataPath)
{
  if (dataPath == m_basemapDataPath)
    return;

  m_basemapDataPath = dataPath;
  emit basemapsDataPathChanged();
  emit propertyUpdated(BASEMAP_DIRECTORY_PROPERTYNAME, m_basemapDataPath);
}

void BasemapPickerController::setDefaultBasemap(const QString& defaultBasemap)
{
  if (defaultBasemap == m_defaultBasemap)
    return;

  m_defaultBasemap = defaultBasemap;
  emit propertyUpdated(DEFAULT_BASEMAP_PROPERTYNAME, m_defaultBasemap);
}

void BasemapPickerController::onBasemapDataPathChanged()
{
  QDir basemapsDir(m_basemapDataPath);

  basemapsDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  basemapsDir.setNameFilters(QStringList{"*.tpk"});

  QFileInfoList list = basemapsDir.entryInfoList();
  int index = -1;
  for (const QFileInfo& fInfo : list)
  {
    if (m_tileCacheModel->append(fInfo.filePath()))
      index++;

    if(fInfo.completeBaseName().compare(m_defaultBasemap, Qt::CaseInsensitive) == 0)
      m_defaultBasemapIndex = index;
  }

  emit tileCacheModelChanged();
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

  Toolkit::ToolResourceProvider::instance()->setBasemap(selectedBasemap);

  const QString basemapName = m_tileCacheModel->tileCacheNameAt(row);

  setDefaultBasemap(basemapName);
  emit basemapChanged(selectedBasemap, basemapName);
}

void BasemapPickerController::selectInitialBasemap()
{
  basemapSelected(m_defaultBasemapIndex);
}

QString BasemapPickerController::toolName() const
{
  return QStringLiteral("basemap picker");
}

void BasemapPickerController::setProperties(const QVariantMap& properties)
{
  const QString newDefaultBasemap = properties.value(DEFAULT_BASEMAP_PROPERTYNAME).toString();
  const bool basemapChanged = !newDefaultBasemap.isEmpty() && newDefaultBasemap != m_defaultBasemap;
  if (basemapChanged)
    setDefaultBasemap(newDefaultBasemap);

  const QString newBasemapDataPath = properties.value(BASEMAP_DIRECTORY_PROPERTYNAME).toString();
  const bool dataPathChanged = !newBasemapDataPath.isEmpty() && newBasemapDataPath != m_basemapDataPath;
  if (dataPathChanged)
    setBasemapDataPath(newBasemapDataPath);

  if (dataPathChanged || basemapChanged)
    selectInitialBasemap();
}
