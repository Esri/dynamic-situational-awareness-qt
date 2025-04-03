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

#include "OpenMobileScenePackageController.h"

// C++ API headers
#include "Error.h"
#include "ErrorException.h"
#include "Item.h"
#include "MapTypes.h"
#include "MobileScenePackage.h"
#include "Scene.h"

// Qt headers
#include <QDir>
#include <QFileInfo>
#include <QFuture>
#include <QJsonDocument>
#include <QQmlContext>
#include <QQmlEngine>

// DSA headers
#include "MobileScenePackagesListModel.h"
#include "ToolManager.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

const QString OpenMobileScenePackageController::PACKAGE_DIRECTORY_PROPERTYNAME = "PackageDirectory";
const QString OpenMobileScenePackageController::CURRENT_PACKAGE_PROPERTYNAME = "CurrentPackage";
const QString OpenMobileScenePackageController::SCENE_INDEX_PROPERTYNAME = "SceneIndex";
const QString OpenMobileScenePackageController::MSPK_EXTENSION = ".mspk";
const QString OpenMobileScenePackageController::MMPK_EXTENSION = ".mmpk";

/*!
  \class Dsa::OpenMobileScenePackageController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for opening mobile scene packages.

  \sa Esri::ArcGISRuntime::MobileScenePackage
 */

/*!
  \brief Constructor taking an optional \a parent.
 */
OpenMobileScenePackageController::OpenMobileScenePackageController(QObject* parent /* = nullptr */):
  AbstractTool(parent),
  m_packagesModel(new MobileScenePackagesListModel(this))
{
  ToolManager::instance().addTool(this);
  emit packagesChanged();
}

/*!
  \brief Destructor.
 */
OpenMobileScenePackageController::~OpenMobileScenePackageController()
{
}

/*!
  \brief Returns the name of this tool - \c "open mobile scene package".
 */
QString OpenMobileScenePackageController::toolName() const
{
  return QStringLiteral("open mobile scene package");
}

/*! \brief Sets any values in \a properties which are relevant for the basemap picker controller.
 *
 * This tool will use the following key/value pairs in the \a properties map if they are set:
 *
 * \list
 *  \li PackageDirectory. The directory containing package data.
 *  \li CurrentPackage. The name of the current package.
 *  \li SceneIndex. The index of the scene in the current package.
 * \endlist
 */
void OpenMobileScenePackageController::setProperties(const QVariantMap& properties)
{
  const QString newPackageDirectoryPath = properties.value(PACKAGE_DIRECTORY_PROPERTYNAME).toString();
  const bool dataPathChanged = setPackageDataPath(newPackageDirectoryPath);

  const QString newPackageName = properties.value(CURRENT_PACKAGE_PROPERTYNAME).toString();
  const bool packageNameChanged = setCurrentPackageName(newPackageName);

  bool ok = false;
  const int newPackageIndex = properties.value(SCENE_INDEX_PROPERTYNAME).toInt(&ok);
  const bool packageIndexChanged = ok && setCurrentSceneIndex(newPackageIndex);

  if ((dataPathChanged || packageNameChanged)
      && !m_currentPackageName.isEmpty()
      && !m_packageDataPath.isEmpty())
  {
    findPackage();
  }
  else if (packageIndexChanged)
  {
    loadScene();
  }
}

/*!
  \internal

  Find the mobile scene package in the packages directory with the current name.
 */
void OpenMobileScenePackageController::findPackage()
{
  const QString packagePath = combinedPackagePath();
  if (!QFileInfo::exists(packagePath))
  {
    emit toolErrorOccurred("Failed to open package", QString("%1 not found").arg(packagePath));
    return;
  }

  QFileInfo packagePathFileInfo(packagePath);
  if (packagePathFileInfo.isDir())
  {
    loadMobileScenePackage(m_currentPackageName);
  }
  else if (packagePath.endsWith(MSPK_EXTENSION))
  {
    loadMobileScenePackage(m_currentPackageName);
  }
  else if (packagePath.endsWith(MMPK_EXTENSION))
  {
    emit toolErrorOccurred("MobileMapPackages (.mmpk) are not supported", packagePath);
    return;
  }
}

/*!
  \internal

  Load the scene at the current index from the current mobile scene package.
 */
void OpenMobileScenePackageController::loadScene()
{
  if (!m_mspk)
    return;

  if (m_currentSceneIndex == -1)
    return;

  const QList<Scene*> scenes = m_mspk->scenes();
  if (scenes.isEmpty())
  {
    emit toolErrorOccurred("Package contains no scenes", combinedPackagePath());
    return;
  }

  // If the index is invalid for this package, fall back to 0
  if (m_currentSceneIndex >= scenes.length())
    return;

  // Get the scene at the current index and add add it to the ToolResourceProvider
  Scene* theScene = scenes.at(m_currentSceneIndex);
  ToolResourceProvider::instance()->setScene(theScene);
}

/*!
  \brief Selects the package with the name \a newPackageName.
 */
void OpenMobileScenePackageController::selectPackageName(const QString& newPackageName)
{
  if (!setCurrentPackageName(newPackageName))
    return;

  // user selected the package
  m_userSelected = true;

  // reset the scene index to -1
  setCurrentSceneIndex(-1);

  // attempt to find the package with the given name.
  findPackage();
}

/*!
  \brief Selects the scene with the index \a newSceneIndex from the current package.

  If succesful, the scene will be loaded.
 */
void OpenMobileScenePackageController::selectScene(int newSceneIndex)
{
  setCurrentSceneIndex(newSceneIndex);

  loadScene();
}

/*!
  \brief Returns the path to the directory where package data is stored.
 */
QString OpenMobileScenePackageController::packageDataPath() const
{
  return m_packageDataPath;
}

/*!
  \brief Sets the path to the directory where package data is stored to \a dataPath.
 */
bool OpenMobileScenePackageController::setPackageDataPath(const QString& dataPath)
{
  if (dataPath == m_packageDataPath || dataPath.isEmpty())
    return false;

  QFileInfo packageDirFileInfo(dataPath);
  if (!packageDirFileInfo.isDir())
    return false;

  m_packageDataPath = dataPath;
  emit packageDataPathChanged();
  emit propertyChanged(PACKAGE_DIRECTORY_PROPERTYNAME, m_packageDataPath);

  // refresh the list of packages
  updatePackageDetails();

  return true;
}

/*!
  \property
  \brief The name of the current mobile scene package.
 */
QString OpenMobileScenePackageController::currentPackageName() const
{
  return m_currentPackageName;
}

/*!
  \internal
  Sets the current package name to \a packageName.
 */
bool OpenMobileScenePackageController::setCurrentPackageName(const QString& packageName)
{
  if (packageName.isEmpty() || packageName == currentPackageName())
    return false;

  m_currentPackageName = packageName;
  emit currentSceneNameChanged();
  emit propertyChanged(CURRENT_PACKAGE_PROPERTYNAME, m_currentPackageName);

  return true;
}

/*!
  \property
  \brief The index of the scene in the current mobile scene package.
 */
int OpenMobileScenePackageController::currentSceneIndex() const
{
  return m_currentSceneIndex;
}

/*!
  \internal
  Loads the current scene index in the supplied \a package if it is the current mobile scene package.
 */
void OpenMobileScenePackageController::loadCurrentScene(MobileScenePackage* package)
{
  if (!m_mspk)
    return;

  if (package == m_mspk)
    loadScene();
}

/*!
  \internal
  Loads the mobile scene package called \a packageName.

  Updates the model with details of the package contents.
 */
void OpenMobileScenePackageController::loadMobileScenePackage(const QString& packageName)
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

    const auto& packageTitle = packageItem->title();
    m_packagesModel->setTitleAndDescription(packageName, packageTitle, packageItem->description());

    auto scenes = package->scenes();
    QStringList sceneNames;
    sceneNames.reserve(scenes.length());
    for (auto* scene : scenes)
    {
      if (!scene)
        continue;

      auto item = scene->item();
      if (!item)
        continue;

      // TODO: this should use Scene::Item::name when available (requires the scene to be loaded)
      sceneNames.append(packageTitle);
      if (!item->thumbnail().isNull())
        continue;

      item->fetchThumbnailAsync().then(this, [this, packageName, packageTitle](QImage image)
      {
        emit imageReady(packageName + "_" + packageTitle, image);
        m_packagesModel->setSceneImagesReady(packageName, true);
      }).onFailed(this, [this, packageName](const ErrorException& e)
      {
        emit toolErrorOccurred(QString("Unable to fetch thumbnail for package %1:, %2").arg(packageName, e.error().additionalMessage()), QString("Package Error"));
        m_packagesModel->setSceneImagesReady(packageName, false);
      });
    }

    m_packagesModel->setSceneNames(packageName, sceneNames);
    package->item()->fetchThumbnailAsync().then(this, [this, packageName](QImage image)
    {
      emit imageReady(packageName, image);
      m_packagesModel->setSceneImagesReady(packageName, true);
    }).onFailed(this, [this, packageName](const ErrorException& e)
    {
      emit toolErrorOccurred(QString("Unable to fetch thumbnail for package %1:, %2").arg(packageName, e.error().additionalMessage()), QString("Package Error"));
      m_packagesModel->setSceneImagesReady(packageName, false);
    });

    loadCurrentScene(package);
  });

  if (package->loadStatus() == LoadStatus::Loaded)
    loadCurrentScene(package);
  else
    package->load();
}

/*!
  \internal
  Creates place-holder details for the given \a packageName.
 */
bool OpenMobileScenePackageController::createPackageDetails(const QString& packageName)
{
  if (m_packages.constFind(packageName) != m_packages.constEnd())
    return false;

  m_packages.insert(packageName, nullptr);
  m_packagesModel->addPackageData(packageName);

  return true;
}

/*!
  \internal
  Returns the path of the current mobile scene package.
 */
QString OpenMobileScenePackageController::combinedPackagePath() const
{
  return m_packageDataPath + "/" + m_currentPackageName;
}

/*!
  \property
  \l brief The list of package details.
 */
QAbstractListModel* OpenMobileScenePackageController::packages() const
{
  return m_packagesModel;
}

/*!
  \brief Indicates whether the user selected the scene or if it was the default.

  By default, the value is \c false.
*/
bool OpenMobileScenePackageController::userSelected() const
{
  return m_userSelected;
}

/*!
  \internal
  Returns the \a packageName as a path in the directory.
 */
QString OpenMobileScenePackageController::pathInPackagesDirectory(const QString& packageName) const
{
  return QDir(m_packageDataPath).filePath(packageName);
}

/*!
  \internal
  Returns the mobile scene pacakge called \a packageName if it exists.
 */
MobileScenePackage *OpenMobileScenePackageController::getPackage(const QString& packageName)
{
  auto findPackage = m_packages.find(packageName);

  MobileScenePackage* package = nullptr;
  if (findPackage == m_packages.end() || findPackage.value() == nullptr)
  {
    package = new MobileScenePackage(pathInPackagesDirectory(packageName), this);
    m_packages.insert(packageName, package);
  }
  else
  {
    package = findPackage.value();
  }

  return package;
}

/*!
  \internal
  Sets the current scene index to \a packageIndex. The scene at the index will be loaded.
 */
bool OpenMobileScenePackageController::setCurrentSceneIndex(int packageIndex)
{
  if (packageIndex == m_currentSceneIndex)
    return false;

  m_currentSceneIndex = packageIndex;
  emit packageIndexChanged();
  emit propertyChanged(SCENE_INDEX_PROPERTYNAME, m_currentSceneIndex);

  loadScene();

  return true;
}

/*!
  \internal
  Updates the details for the packages in the \l packageDataPath.

  Packages will be tested to see if they can be read directly, and if so,
  loaded to obtaiin meta-data and images.
 */
void OpenMobileScenePackageController::updatePackageDetails()
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

    loadMobileScenePackage(packageName);
  }

  for (const auto& packageName : dirPackageNames)
  {
    // existing package
    if (!createPackageDetails(packageName))
      continue;

    loadMobileScenePackage(packageName);
  }

}

/*!
 * \brief Returns \c true when there is a valid scene package
 * and scene index cached in this object.
 */
bool OpenMobileScenePackageController::hasActiveScene() const
{
  return m_mspk && m_currentSceneIndex >= 0;
}

} // Dsa

// Signal Documentation



/*!
  \fn void OpenMobileScenePackageController::toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);

  \brief Signal emitted an error occurs.
 */

/*!
  \fn void OpenMobileScenePackageController::packageDataPathChanged();

  \brief Signal emitted when the \l packageDataPath property changes.
 */

/*!
  \fn void OpenMobileScenePackageController::currentSceneNameChanged();

  \brief Signal emitted when the \l currentSceneName property changes.
 */

/*!
  \fn void OpenMobileScenePackageController::packageIndexChanged();

  \brief Signal emitted when the \l packageIndex property changes.
 */

/*!
  \fn void OpenMobileScenePackageController::imageReady(const QString& packageName, const QImage& packageImage);

  \brief Signal emitted when the \a packageImage for the package \a packageName is ready.
 */

/*!
  \fn void OpenMobileScenePackageController::packagesChanged();

  \brief Signal emitted when the \a packages model property changes.
 */
