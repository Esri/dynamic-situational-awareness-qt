
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

#include "OpenPackageController.h"

// toolkit headers
#include "ToolManager.h"

// C++ API headers
#include "MobileScenePackage.h"
#include "Scene.h"

// Qt headers
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString OpenPackageController::PACKAGE_DIRECTORY_PROPERTYNAME = "PackageDirectory";
const QString OpenPackageController::CURRENT_PACKAGE_PROPERTYNAME = "CurrentPackage";
const QString OpenPackageController::PACKAGE_INDEX_PROPERTYNAME = "PackageIndex";
const QString OpenPackageController::MSPK_EXTENSION = ".mspk";
const QString OpenPackageController::MMPK_EXTENSION = ".mmpk";

/*!
  \class Dsa::OpenPackageController
  \inmodule Dsa
  \inherits Toolkit::AbstractTool
  \brief Tool controller for opening mobile packages.

  \sa Esri::ArcGISRuntime::MobileScenePackage
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
OpenPackageController::OpenPackageController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(MobileScenePackage::instance(), &MobileScenePackage::errorOccurred, this, &OpenPackageController::errorOccurred);

  connect(MobileScenePackage::instance(), &MobileScenePackage::isDirectReadSupportedCompleted, this, [this]
          (QUuid, bool directReadSupported)
  {
    if (!directReadSupported)
    {
      // needs unpacked before loading
      QString unpackedPackageName = m_currentPackageName;
      unpackedPackageName.replace(MSPK_EXTENSION, "_unpacked");
      const QString unpackedDir = m_packageDataPath + "/" + unpackedPackageName;

      // If a directory with the unpacked name already exists, usue that
      if (QFileInfo::exists(unpackedDir))
      {
        qDebug() << "Loading unpacked version " << unpackedDir;
        loadMobileScenePackage(unpackedDir);
        setCurrentPackageName(unpackedPackageName);

        return;
      }

      MobileScenePackage::unpack(combinedPackagePath(), unpackedDir);
    }
    else
    {
      // proceed with loading the archived mspk file
      loadMobileScenePackage(combinedPackagePath());
    }
  });

  connect(MobileScenePackage::instance(), &MobileScenePackage::unpackCompleted, this, [this](QUuid, bool success)
  {
    if (!success)
    {
      emit toolErrorOccurred("Failed to unpack mspk", "Could not unpack");
      return;
    }

    refreshPackageNames();

    QString unpackedPackageName = m_currentPackageName;
    unpackedPackageName.replace(MSPK_EXTENSION, "_unpacked");
    const QString unpackedDir = m_packageDataPath + "/" + unpackedPackageName;
    loadMobileScenePackage(unpackedDir);
    setCurrentPackageName(unpackedPackageName);
  });
}

/*!
  \brief Destructor.
 */
OpenPackageController::~OpenPackageController()
{
}

/*!
  \brief Returns the name of this tool - \c "mobile scene package picker".
 */
QString OpenPackageController::toolName() const
{
  return QStringLiteral("mobile scene package picker");
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
void OpenPackageController::setProperties(const QVariantMap& properties)
{
  const QString newPackageDirectoryPath = properties.value(PACKAGE_DIRECTORY_PROPERTYNAME).toString();
  const bool dataPathChanged = setPackageDataPath(newPackageDirectoryPath);

  const QString newPackageName = properties.value(CURRENT_PACKAGE_PROPERTYNAME).toString();
  const bool packageNameChanged = setCurrentPackageName(newPackageName);

  bool ok = false;
  const int newPackageIndex = properties.value(PACKAGE_INDEX_PROPERTYNAME).toInt(&ok);
  const bool packageIndexChanged = ok && setPackageIndex(newPackageIndex);

  if ((dataPathChanged || packageNameChanged)
      && !m_currentPackageName.isEmpty()
      && !m_packageDataPath.isEmpty())
  {
    findPackage();
  }
  else if (packageIndexChanged)
  {
    // load the new scene
  }
}

void OpenPackageController::findPackage()
{
  const QString packagePath = combinedPackagePath();
  QFileInfo packagePathFileInfo = packagePath;
  if (!packagePathFileInfo.exists())
  {
    emit toolErrorOccurred("Failed to open package", QString("%1 not found").arg(packagePath));
    return;
  }

  if (packagePathFileInfo.isDir())
  {
    // package is already unpacked
    loadMobileScenePackage(packagePath);
  }
  else if (packagePath.endsWith(MSPK_EXTENSION))
  {
    MobileScenePackage::isDirectReadSupported(packagePath);
  }
  else if(packagePath.endsWith(MMPK_EXTENSION))
  {
    emit toolErrorOccurred("MobileMapPackages (.mmpk) are not supported", packagePath);
    return;
  }
}

void OpenPackageController::loadGeoDocument()
{
  if (!m_mspk)
    return;

  const QList<Scene*> scenes = m_mspk->scenes();
  if (scenes.isEmpty())
  {
    emit toolErrorOccurred("Package contaisn no scenes", combinedPackagePath());
    return;
  }

  // If the index is invalid for this package, fall back to 0
  if (m_packageIndex >= scenes.length())
  {
    setPackageIndex(0);
  }

  Scene* theScene = scenes.at(m_packageIndex);
  Toolkit::ToolResourceProvider::instance()->setScene(theScene);
}

void OpenPackageController::selectPackageName(QString newPackageName)
{
  if (!setCurrentPackageName(newPackageName))
    return;

  findPackage();
}

QString OpenPackageController::packageDataPath() const
{
  return m_packageDataPath;
}

bool OpenPackageController::setPackageDataPath(const QString dataPath)
{
  if (dataPath == m_packageDataPath || dataPath.isEmpty())
    return false;

  QFileInfo packageDirFileInfo(dataPath);
  if (!packageDirFileInfo.isDir())
    return false;

  m_packageDataPath = std::move(dataPath);
  emit packageDataPathChanged();

  refreshPackageNames();

  return true;
}

QString OpenPackageController::currentPackageName() const
{
  return m_currentPackageName;
}

bool OpenPackageController::setCurrentPackageName(const QString packageName)
{
  if (packageName.isEmpty() || packageName == currentPackageName())
    return false;

  m_currentPackageName = std::move(packageName);
  emit currentPackageNameChanged();
  emit propertyChanged(CURRENT_PACKAGE_PROPERTYNAME, m_currentPackageName);

  return true;
}

int OpenPackageController::packageIndex() const
{
  return m_packageIndex;
}

void OpenPackageController::loadMobileScenePackage(const QString& mspkPath)
{
  MobileScenePackage* oldPackage = nullptr;
  if (m_mspk != nullptr)
  {
    oldPackage = m_mspk;
  }

  Scene* oldScene = Toolkit::ToolResourceProvider::instance()->scene();
  {
    Toolkit::ToolResourceProvider::instance()->setScene(nullptr);
    delete oldScene;
  }

  m_mspk = new MobileScenePackage(mspkPath, this);
  connect(m_mspk, &MobileScenePackage::errorOccurred, this, &OpenPackageController::errorOccurred);

  connect(m_mspk, &MobileScenePackage::doneLoading, this, [this, oldPackage](Error e)
  {
    if (!e.isEmpty())
    {
      return;
    }

    // load the scene from the specified index
    loadGeoDocument();

    if (oldPackage)
    {
      // TODO: force close MSPK and properly clean-up
      delete oldPackage;
    }
  });

  m_mspk->load();
}

QString OpenPackageController::combinedPackagePath() const
{
 return m_packageDataPath + "/" + m_currentPackageName;
}

QStringList OpenPackageController::packageNames() const
{
  return m_packageNames;
}

void OpenPackageController::setPackageNames(QStringList packageNames)
{
  if (packageNames == m_packageNames)
    return;

  m_packageNames = std::move(packageNames);

  emit packageNamesChanged();
}

bool OpenPackageController::setPackageIndex(int packageIndex)
{
  if (packageIndex == m_packageIndex || packageIndex < 0)
    return false;

  m_packageIndex = packageIndex;
  emit packageIndexChanged();

  return true;
}

void OpenPackageController::refreshPackageNames()
{
  QDir dir(m_packageDataPath);
  QStringList filters { QString("*" + MSPK_EXTENSION) };
  dir.setNameFilters(filters); // filter to include on .mspk files
  QStringList packageNames = dir.entryList();
  dir.setFilter(QDir::AllDirs |
                QDir::NoDot |
                QDir::NoDotDot); // filter to include all child directories (for unpacked mspk)
  packageNames.append(dir.entryList());

  setPackageNames(packageNames);
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

