
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

    updatePackageDetails();

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
  {
    setCurrentDocumentIndex(0);
  }

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
      loadMobileScenePackageForDetails(packageName);

    return;
  }

  // otherwise, if the task was concerned with loading a package for display
  if (directReadSupported)
  {
    // proceed with loading the archived mspk file
    loadMobileScenePackage(combinedPackagePath());
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

bool OpenPackageController::setCurrentPackageName(const QString packageName)
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

void OpenPackageController::loadMobileScenePackageForDetails(const QString& packageName)
{
  MobileScenePackage* newPackage = new MobileScenePackage(m_packageDataPath + "/" + packageName, this);
  m_packages.insert(packageName, newPackage);

  connect(newPackage, &MobileScenePackage::doneLoading, this, [this, newPackage, packageName](Error e)
  {
    if (!e.isEmpty())
    {
      qDebug() << packageName << e.message() << e.additionalMessage();
      return;
    }

    auto packageItem = newPackage->item();
    if (!packageItem)
      return;

    auto scenes = newPackage->scenes();
    QStringList documentNames;
    documentNames.reserve(scenes.length());
    for (int i = 0; i < scenes.length(); ++i)
    {
      // TODO: this should use Scene::Item::name when available (requires the scene to be loaded)
      documentNames.append(QString("Scene %1").arg(QString::number(i)));
    }

    m_packagesModel->setDocumentNames(packageName, documentNames);

    connect(packageItem, &Item::fetchThumbnailCompleted, this, [this, packageName, packageItem](bool success)
    {
      if (success)
        emit imageReady(packageName, packageItem->thumbnail());

      m_packagesModel->setImageReady(packageName, success);
    });

    newPackage->item()->fetchThumbnail();
  });

  newPackage->load();
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

bool OpenPackageController::setCurrentDocumentIndex(int packageIndex)
{
  if (packageIndex == m_currentDocumentIndex)
    return false;

  m_currentDocumentIndex = packageIndex;
  emit packageIndexChanged();
  emit propertyChanged(PACKAGE_INDEX_PROPERTYNAME, m_currentDocumentIndex);

  return true;
}

void OpenPackageController::updatePackageDetails()
{
  QDir dir(m_packageDataPath);
  QStringList filters { QString("*" + MSPK_EXTENSION) };
  dir.setNameFilters(filters); // filter to include on .mspk files
  const QStringList filePackageNames = dir.entryList();
  for (const auto& packageName : filePackageNames)
  {
    // existing package
    if (!createPackageDetails(packageName))
      continue;

    // check if it needs unpack
    const auto taskWatcher = MobileScenePackage::instance()->isDirectReadSupported(m_packageDataPath);
    m_directReadTasks.insert(taskWatcher.taskId(), packageName);
  }

  dir.setFilter(QDir::AllDirs |
                QDir::NoDot |
                QDir::NoDotDot); // filter to include all child directories (for unpacked mspk)
  const QStringList dirPackageNames = dir.entryList();
  for (const auto& packageName : dirPackageNames)
  {
    // existing package
    if (!createPackageDetails(packageName))
      continue;

    loadMobileScenePackageForDetails(packageName);
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

