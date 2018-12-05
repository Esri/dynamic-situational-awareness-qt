
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
#include "PackagesListModel.h"

// toolkit headers
#include "ToolManager.h"

// C++ API headers
#include "Item.h"
#include "MobileScenePackage.h"
#include "Scene.h"
#include "SceneQuickView.h"

// Qt headers
#include <QQmlContext>
#include <QDir>
#include <QQmlEngine>
#include <QFileInfo>
#include <QJsonDocument>
#include <QQmlEngine>

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
  Toolkit::AbstractTool(parent),
  m_packagesModel(new PackagesListModel(this))
{
  emit packagesChanged();
  Toolkit::ToolManager::instance().addTool(this);

  //connect(MobileScenePackage::instance(), &MobileScenePackage::errorOccurred, this, &OpenPackageController::errorOccurred);

  connect(MobileScenePackage::instance(), &MobileScenePackage::isDirectReadSupportedCompleted, this,
          &OpenPackageController::handleIsDirectReadSupportedCompleted);

  connect(MobileScenePackage::instance(), &MobileScenePackage::unpackCompleted, this, [this](QUuid, bool success)
  {
    if (!success)
    {
      emit toolErrorOccurred("Failed to unpack mspk", "Could not unpack");
      return;
    }

    QString unpackedPackageName = getUnpackedName(m_currentPackageName);

    m_packagesModel->setUnpackedName(m_currentPackageName, unpackedPackageName);
    setCurrentPackageName(unpackedPackageName);
    loadMobileScenePackage(unpackedPackageName);

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
  const bool packageIndexChanged = ok && setCurrentDocumentIndex(newPackageIndex);

  if ((dataPathChanged || packageNameChanged)
      && !m_currentPackageName.isEmpty()
      && !m_packageDataPath.isEmpty())
  {
    findPackage();
  }
  else if (packageIndexChanged)
  {
    loadGeoDocument();
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
    loadMobileScenePackage(m_currentPackageName);
  }
  else if (packagePath.endsWith(MSPK_EXTENSION))
  {
    const auto taskWatcher = MobileScenePackage::instance()->isDirectReadSupported(packagePath);
    m_directReadTasks.insert(taskWatcher.taskId(), m_currentPackageName);
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

  if (m_currentDocumentIndex == -1)
    return;

  const QList<Scene*> scenes = m_mspk->scenes();
  if (scenes.isEmpty())
  {
    emit toolErrorOccurred("Package contains no scenes", combinedPackagePath());
    return;
  }

  // If the index is invalid for this package, fall back to 0
  if (m_currentDocumentIndex >= scenes.length())
    return;

  Scene* theScene = scenes.at(m_currentDocumentIndex);
  Toolkit::ToolResourceProvider::instance()->setScene(theScene);
}

void OpenPackageController::selectPackageName(QString newPackageName)
{
  if (!setCurrentPackageName(newPackageName))
    return;

  // reset the scene index to -1
  setCurrentDocumentIndex(-1);

  findPackage();
}

void OpenPackageController::selectDocument(int newDocumentIndex)
{
  if (!setCurrentDocumentIndex(newDocumentIndex))
    return;

  loadGeoDocument();
}

void OpenPackageController::unpack()
{
  // needs unpacked before loading
  QString unpackedPackageName = getUnpackedName(m_currentPackageName);
  const QString unpackedDir = m_packageDataPath + "/" + unpackedPackageName;

  // If a directory with the unpacked name already exists, usue that
  if (QFileInfo::exists(unpackedDir))
  {
    qDebug() << "Loading unpacked version " << unpackedDir;
    setCurrentPackageName(unpackedPackageName);
    loadMobileScenePackage(m_currentPackageName);

    return;
  }

  MobileScenePackage::unpack(combinedPackagePath(), unpackedDir);
}

void OpenPackageController::handleIsDirectReadSupportedCompleted(QUuid taskId, bool directReadSupported)
{
  auto findTask = m_directReadTasks.constFind(taskId);

  // If the task was concerned with checking a packages details
  if (findTask != m_directReadTasks.constEnd())
  {
    // Update whether the package requires unpack
    const auto& packageName = findTask.value();
    m_packagesModel->setRequiresUnpack(packageName, !directReadSupported);

    // If the package doesn't need to eb unpacked, get it's thumbnai;, and scenes etc.
    if (directReadSupported)
      loadMobileScenePackage(packageName);

    return;
  }
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

  updatePackageDetails();

  return true;
}

QString OpenPackageController::currentPackageName() const
{
  return m_currentPackageName;
}

bool OpenPackageController::setCurrentPackageName(QString packageName)
{
  if (packageName.isEmpty() || packageName == currentPackageName())
    return false;

  m_currentPackageName = std::move(packageName);
  emit currentPackageNameChanged();
  emit propertyChanged(CURRENT_PACKAGE_PROPERTYNAME, m_currentPackageName);

  return true;
}

int OpenPackageController::currentDocumentIndex() const
{
  return m_currentDocumentIndex;
}

void OpenPackageController::loadCurrentGeoDocument(MobileScenePackage* package)
{
  if (!m_mspk)
    return;

  if (package == m_mspk)
    loadGeoDocument();
}

void OpenPackageController::loadMobileScenePackage(const QString& packageName)
{
  MobileScenePackage* package = getPackage(packageName);
  if (!package)
    return;

  if (packageName == m_currentPackageName)
    m_mspk = package;

  connect(package, &MobileScenePackage::doneLoading, this, [this, package, packageName](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << packageName << e.message() << e.additionalMessage();
      return;
    }

    auto packageItem = package->item();
    if (!packageItem)
      return;

    auto scenes = package->scenes();
    QStringList documentNames;
    documentNames.reserve(scenes.length());
    for (int i = 0; i < scenes.length(); ++i)
    {
      // TODO: this should use Scene::Item::name when available (requires the scene to be loaded)
      documentNames.append(QString("Scene %1").arg(QString::number(i)));
    }

    // If the package is unpacked, update the details for the original
    const QString packageNameToUse = m_packagesModel->isUnpackedVersion(packageName) ? getPackedName(packageName)
                                                                                     : packageName;

    m_packagesModel->setDocumentNames(packageNameToUse, documentNames);

    connect(packageItem, &Item::fetchThumbnailCompleted, this, [this, packageNameToUse, packageItem](bool success)
    {
      if (success)
        emit imageReady(packageNameToUse, packageItem->thumbnail());

      m_packagesModel->setImageReady(packageNameToUse, success);
    });

    package->item()->fetchThumbnail();

    loadCurrentGeoDocument(package);
  });

  if (package->loadStatus() == LoadStatus::Loaded)
    loadCurrentGeoDocument(package);
  else
    package->load();
}

bool OpenPackageController::createPackageDetails(const QString& packageName)
{
  if (m_packages.constFind(packageName) != m_packages.constEnd())
    return false;

  m_packages.insert(packageName, nullptr);
  m_packagesModel->addPackageData(packageName);

  return true;
}

QString OpenPackageController::combinedPackagePath() const
{
  return m_packageDataPath + "/" + m_currentPackageName;
}

QAbstractListModel* OpenPackageController::packages() const
{
  return m_packagesModel;
}

QString OpenPackageController::getPackedName(const QString& packageName)
{
  QString packedPackageName = packageName;
  packedPackageName.replace("_unpacked", MSPK_EXTENSION);

  return packedPackageName;
}

QString OpenPackageController::getUnpackedName(const QString& packageName)
{
  QString unpackedPackageName = packageName;
  unpackedPackageName.replace(MSPK_EXTENSION, "_unpacked");

  return unpackedPackageName;
}

MobileScenePackage *OpenPackageController::getPackage(const QString &packageName)
{
  auto findPackage = m_packages.find(packageName);

  MobileScenePackage* package = nullptr;
  if (findPackage == m_packages.end() || findPackage.value() == nullptr)
  {
    package = new MobileScenePackage(m_packageDataPath + "/" + packageName, this);
    m_packages.insert(packageName, package);
  }
  else
  {
    package = findPackage.value();
  }

  return package;
}

bool OpenPackageController::setCurrentDocumentIndex(int packageIndex)
{
  if (packageIndex == m_currentDocumentIndex)
    return false;

  m_currentDocumentIndex = packageIndex;
  emit packageIndexChanged();
  emit propertyChanged(PACKAGE_INDEX_PROPERTYNAME, m_currentDocumentIndex);

  loadGeoDocument();

  return true;
}

void OpenPackageController::updatePackageDetails()
{
  QDir dir(m_packageDataPath);
  QStringList filters { QString("*" + MSPK_EXTENSION) };
  dir.setNameFilters(filters); // filter to include on .mspk files
  const QStringList filePackageNames = dir.entryList();

  dir.setFilter(QDir::AllDirs |
                QDir::NoDot |
                QDir::NoDotDot); // filter to include all child directories (for unpacked mspk)
  auto dirPackageNames = dir.entryList();

  for (const auto& packageName : filePackageNames)
  {
    // existing package
    if (!createPackageDetails(packageName))
      continue;

    const QString unpackedName = getUnpackedName(packageName);
    // if the package is already unpacked, record the unpacked name
    if (dirPackageNames.contains(unpackedName))
    {
      m_packagesModel->setUnpackedName(packageName, unpackedName);
      dirPackageNames.removeAll(unpackedName);

      // try and load the unpacked version
      loadMobileScenePackage(unpackedName);
    }
    else
    {
      // check if it needs unpack
      const auto taskWatcher = MobileScenePackage::instance()->isDirectReadSupported(m_packageDataPath + "/" + packageName);
      m_directReadTasks.insert(taskWatcher.taskId(), packageName);
    }
  }

  for (const auto& packageName : dirPackageNames)
  {
    // existing package
    if (!createPackageDetails(packageName))
      continue;

    loadMobileScenePackage(packageName);
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

