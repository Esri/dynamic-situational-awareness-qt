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

#include "MobileScenePackagesListModel.h"

namespace Dsa {

/*!
  \class Dsa::MobileScenePackagesListModel
  \inmodule Dsa
  \inherits QAbstractListModel
  \brief A model for storing details about mobile scene packages
  available to the app.

  The model returns data for the following roles:
  \table
    \header
        \li Role
        \li Type
        \li Description
    \row
        \li packageName
        \li QString
        \li The file name of the package.
    \row
        \li imageReady
        \li bool
        \li Whether a thumbnail image for the package is ready.
    \row
        \li sceneNames
        \li QStringList
        \li The names of the scenes contained in the package.
    \row
        \li requiresUnpack
        \li bool
        \li Whether the package must be unpacked before it can be loaded.
    \row
        \li unpackedName
        \li QString
        \li The name of the unpacked version of this package, if it exists.
    \row
        \li sceneImagesReady
        \li bool
        \li Whether thumbnail imagea for the scenes in the package are ready.
    \row
        \li packageTitle
        \li QString
        \li The title of the packagem if set.
    \row
        \li packageDescription
        \li QString
        \li The description of the package, if set.
  \endtable
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
MobileScenePackagesListModel::MobileScenePackagesListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[PackageNameRole] = "packageName";
  m_roles[ImageReadyRole] = "imageReady";
  m_roles[SceneNamesRole] = "sceneNames";
  m_roles[RequiresUnpackRole] = "requiresUnpack";
  m_roles[UnpackedNameRole] = "unpackedName";
  m_roles[SceneImagesReadyRole] = "sceneImagesReady";
  m_roles[PackageTitleRole] = "packageTitle";
  m_roles[PackageDescriptionRole] = "packageDescription";
}

/*!
  \brief Destructor.
 */
MobileScenePackagesListModel::~MobileScenePackagesListModel()
{
}

/*!
  \brief Add a data row for the package called \a packageName.
 */
void MobileScenePackagesListModel::addPackageData(const QString& packageName)
{
  beginResetModel();
  m_packageDetails.insert(packageName, PackageDetails());
  endResetModel();
}

/*!
  \brief Remove the data for the package called \a packageName.
 */
void MobileScenePackagesListModel::removePackageDetails(const QString& packageName)
{
  beginResetModel();
  m_packageDetails.remove(packageName);
  endResetModel();
}

/*!
  \internal
  Emit the \l dataChanged signal for the row referenced by \a changedIterator.
 */
void MobileScenePackagesListModel::broadcastDataChanged(const QMap<QString, PackageDetails>::iterator& changedIterator)
{
  int index = std::distance(m_packageDetails.begin(), changedIterator);
  auto changedIndex = createIndex(index, 0);
  emit dataChanged(changedIndex, changedIndex);
}

/*!
  \brief Update the data for \a packageName, setting whether the package requires to be unpacked to \a requiresUnpack.
 */
void MobileScenePackagesListModel::setRequiresUnpack(const QString& packageName, bool requiresUnpack)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_requiresUnpack = requiresUnpack;

  broadcastDataChanged(findIt);
}

/*!
  \brief Update the data for \a packageName, setting whether an image is available to \a imageReady.
 */
void MobileScenePackagesListModel::setImageReady(const QString& packageName, bool imageReady)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_imageReady = imageReady;
  broadcastDataChanged(findIt);
}

/*!
  \brief Update the data for \a packageName, setting the list of scene names to \a sceneNames.
 */
void MobileScenePackagesListModel::setSceneNames(const QString& packageName, QStringList sceneNames)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_sceneNames = std::move(sceneNames);

  broadcastDataChanged(findIt);
}

/*!
  \brief Update the data for \a packageName, setting the unpacked name to \a unpackedName.
 */
void MobileScenePackagesListModel::setUnpackedName(const QString &packageName, QString unpackedName)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_unpackedName = std::move(unpackedName);
  // since an unpacked version exists, requires unpack should be set to false
  findIt.value().m_requiresUnpack = false;

  broadcastDataChanged(findIt);
}

/*!
  \brief Update the data for \a packageName, setting whether images are ready for the scenes to \a sceneImagesReady.
 */
void MobileScenePackagesListModel::setSceneImagesReady(const QString& packageName, bool sceneImagesReady)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_sceneImagesReady = sceneImagesReady;

  broadcastDataChanged(findIt);
}

/*!
  \brief Update the data for \a packageName, setting the title to \a title and the description to \a description.
 */
void MobileScenePackagesListModel::setTitleAndDescription(const QString& packageName, QString title, QString description)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_title = title;
  findIt.value().m_description = description;

  broadcastDataChanged(findIt);
}

/*!
  \brief Returns whether \a packageName is already present as the unpacked version of a package file.
 */
bool MobileScenePackagesListModel::isUnpackedVersion(const QString& packageName) const
{
  auto it = m_packageDetails.constBegin();
  auto itEnd = m_packageDetails.constEnd();
  for (; it != itEnd; ++it)
  {
    if (it.value().m_unpackedName == packageName)
      return true;
  }

  return false;
}

/*!
  \brief Returns the number of package details in the model.

  /list
  /li /a parent - The parent object.
  /endlist

 */
int MobileScenePackagesListModel::rowCount(const QModelIndex&) const
{
  return m_packageDetails.size();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l PackageRoles enum.
 */
QVariant MobileScenePackagesListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return QVariant();

  auto it = m_packageDetails.constBegin();
  it.operator +=(index.row());

  switch (role)
  {
  case PackageNameRole:
    return it.key();
  case ImageReadyRole:
    return it.value().m_imageReady;
  case SceneNamesRole:
    return it.value().m_sceneNames;
  case RequiresUnpackRole:
    return it.value().m_requiresUnpack;
  case UnpackedNameRole:
    return it.value().m_unpackedName;
  case SceneImagesReadyRole:
    return it.value().m_sceneImagesReady;
  case PackageTitleRole:
    return it.value().m_title;
  case PackageDescriptionRole:
    return it.value().m_description;
  default:
    break;
  }

  return QVariant();
}

/*!
  \brief Returns the hash of role names used by the model.

  The roles are based on the \l DataItemRoles enum.
 */
QHash<int, QByteArray> MobileScenePackagesListModel::roleNames() const
{
  return m_roles;
}

} // Dsa
