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

// PCH header
#include "pch.hpp"

#include "BasemapPickerController.h"

// example app headers
#include "TileCacheListModel.h"

// toolkit headers
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "ArcGISTiledLayer.h"
#include "Basemap.h"

// Qt headers
#include <QDir>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString BasemapPickerController::DEFAULT_BASEMAP_PROPERTYNAME = "DefaultBasemap";
const QString BasemapPickerController::BASEMAP_DIRECTORY_PROPERTYNAME = "BasemapDirectory";

/*!
  \class Dsa::BasemapPickerController
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Tool controller for setting a basemap for the app.

  \sa Esri::ArcGISRuntime::Basemap
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
BasemapPickerController::BasemapPickerController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_tileCacheModel(new TileCacheListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(this, &BasemapPickerController::basemapsDataPathChanged, this, &BasemapPickerController::onBasemapDataPathChanged);
}

/*!
  \brief Destructor.
 */
BasemapPickerController::~BasemapPickerController()
{
}

/*!
  \brief Sets the \a dataPath where local basemaps (.tpk files) are located.
 */
void BasemapPickerController::setBasemapDataPath(const QString& dataPath)
{
  if (dataPath == m_basemapDataPath)
    return;

  m_basemapDataPath = dataPath;
  emit basemapsDataPathChanged();
  emit propertyChanged(BASEMAP_DIRECTORY_PROPERTYNAME, m_basemapDataPath);
}

/*!
  \brief Sets the name of the default basemap to \a defaultBasemap.
 */
void BasemapPickerController::setDefaultBasemap(const QString& defaultBasemap)
{
  if (defaultBasemap == m_defaultBasemap)
    return;

  m_defaultBasemap = defaultBasemap;
  emit propertyChanged(DEFAULT_BASEMAP_PROPERTYNAME, m_defaultBasemap);
}

/*!
  \brief Handle changes to the basemap data path.

  When the path is changed, a list of tile cache files (.tpk) located
  in the directory is built up.
 */
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
    emit toolErrorOccurred(QString("Default Basemap not found: %1").arg(m_defaultBasemap), QString("Failed to find %1").arg(m_defaultBasemap));

  emit tileCacheModelChanged();
}

/*!
  \property Dsa::BasemapPickerController::tileCacheModel
  \brief Returns a model of the local tile cache files contained in the
  basemap data directory.
 */
QAbstractListModel* BasemapPickerController::tileCacheModel() const
{
  return m_tileCacheModel;
}

/*!
  \brief Selects the basemap at index \a row in the \l tileCacheModel and
  sets it on the view.
 */
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

/*!
  \brief Selects the initial basemap.
 */
void BasemapPickerController::selectInitialBasemap()
{
  basemapSelected(m_defaultBasemapIndex);
}

/*!
  \brief Returns the name of this tool - \c "basemap picker".
 */
QString BasemapPickerController::toolName() const
{
  return QStringLiteral("basemap picker");
}

/*! \brief Sets any values in \a properties which are relevant for the basemap picker controller.
 *
 * This tool will use the following key/value pairs in the \a properties map if they are set:
 *
 * \list
 *  \li DefaultBasemap. The name of the default basemap to load.
 *  \li BasemapDirectory. The directory containing basemap data.
 * \endlist
 */
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

} // Dsa

// Signal Documentation

/*!
  \fn void BasemapPickerController::tileCacheModelChanged();

  \brief Signal emitted when the TileCacheModel associated with this class changes.
 */

/*!
  \fn void BasemapPickerController::basemapsDataPathChanged();

  \brief Signal emitted when basemap data path changes.
 */

/*!
  \fn void BasemapPickerController::basemapChanged(Esri::ArcGISRuntime::Basemap* basemap, QString name = "");

  \brief Signal emitted when the current \a basemap changes.

  The \a name of the basemap is passed through the signal as a parameter.
 */

/*!
  \fn void BasemapPickerController::toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

  \brief Signal emitted when an error occurs.

  An \a errorMessage and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */

