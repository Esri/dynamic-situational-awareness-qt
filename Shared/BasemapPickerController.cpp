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
  emit propertyChanged(BASEMAP_DIRECTORY_PROPERTYNAME, m_basemapDataPath);
}

void BasemapPickerController::setDefaultBasemap(const QString& defaultBasemap)
{
  if (defaultBasemap == m_defaultBasemap)
    return;

  m_defaultBasemap = defaultBasemap;
  emit propertyChanged(DEFAULT_BASEMAP_PROPERTYNAME, m_defaultBasemap);
}

void BasemapPickerController::onBasemapDataPathChanged()
{
  m_tileCacheModel->clear();

  QDir basemapsDir(m_basemapDataPath);
  if (!basemapsDir.exists())
  {
    emit toolErrorOccurred(QString("Could not find Basemaps dir %1").arg(basemapsDir.dirName()), QString("Failed to find %1").arg(m_basemapDataPath));
    return;
  }

  basemapsDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  basemapsDir.setNameFilters(QStringList{"*.tpk"});

  QFileInfoList list = basemapsDir.entryInfoList();
  if (list.isEmpty())
  {
    emit toolErrorOccurred(QString("Empty Basemaps dir %1").arg(basemapsDir.dirName()), QString("No .tpk files in %1").arg(m_basemapDataPath));
    return;
  }

  int index = -1;
  bool foundDefault = false;
  for (const QFileInfo& fInfo : list)
  {
    if (m_tileCacheModel->append(fInfo.filePath()))
      index++;

    if(!foundDefault && fInfo.completeBaseName().compare(m_defaultBasemap, Qt::CaseInsensitive) == 0)
    {
      m_defaultBasemapIndex = index;
      foundDefault = true;
    }
  }

  if (!foundDefault)
    emit toolErrorOccurred(QString("Deafault Basemap not found: %1").arg(m_defaultBasemap), QString("Failed to find %1").arg(m_defaultBasemap));

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
  {
    onBasemapDataPathChanged();
    selectInitialBasemap();
  }
}
