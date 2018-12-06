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
  \class Dsa::TileCacheListModel
  \inmodule Dsa
  \inherits QAbstractListModel
  \brief A model for storing the list of
  \l Esri::ArcGISRuntime::TileCache files available for
  use as basemaps in the app.

  The model returns data for the following roles:
  \table
    \header
        \li Role
        \li Type
        \li Description
    \row
        \li title
        \li QString
        \li The title of the tile cache.
    \row
        \li fileName
        \li QString
        \li The file path to the tile cache.
    \row
        \li thumbnailUrl
        \li QUrl
        \li The URL to the thumbnail of the tile cache.
  \endtable
 */

/*!
  \brief Constructor for a model taking an optional \a parent.
 */
MobileScenePackagesListModel::PackagesListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[PackageNameRole] = "packageName";
  m_roles[ImageReadyRole] = "imageReady";
  m_roles[DocumentsRole] = "documents";
  m_roles[RequiresUnpackRole] = "requiresUnpack";
  m_roles[UnpackedNameRole] = "unpackedName";
}

/*!
  \brief Destructor.
 */
PackagesListModel::~PackagesListModel()
{
}

void PackagesListModel::addPackageData(const QString& packageName)
{
  beginResetModel();
  m_packageDetails.insert(packageName, PackageDetails());
  endResetModel();
}

void PackagesListModel::removePackageDetails(const QString& packageName)
{
  beginResetModel();
  m_packageDetails.remove(packageName);
  endResetModel();
}

void PackagesListModel::setRequiresUnpack(const QString& packageName, bool requiresUnpack)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_requiresUnpack = requiresUnpack;

  int index = std::distance(m_packageDetails.begin(), findIt);
  auto changedIndex = createIndex(index, 0);
  emit dataChanged(changedIndex, changedIndex);
}

void PackagesListModel::setImageReady(const QString& packageName, bool imageReady)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_imageReady = imageReady;

  int index = std::distance(m_packageDetails.begin(), findIt);
  auto changedIndex = createIndex(index, 0);
  emit dataChanged(changedIndex, changedIndex);
}

void PackagesListModel::setDocumentNames(const QString& packageName, QStringList documentNames)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_documentNames = std::move(documentNames);

  int index = std::distance(m_packageDetails.begin(), findIt);
  auto changedIndex = createIndex(index, 0);
  emit dataChanged(changedIndex, changedIndex);
}

void PackagesListModel::setUnpackedName(const QString &packageName, QString unpackedName)
{
  auto findIt = m_packageDetails.find(packageName);
  if (findIt == m_packageDetails.end())
    return;

  findIt.value().m_unpackedName = std::move(unpackedName);
  findIt.value().m_requiresUnpack = false;

  int index = std::distance(m_packageDetails.begin(), findIt);
  auto changedIndex = createIndex(index, 0);
  emit dataChanged(changedIndex, changedIndex);
}

bool PackagesListModel::isUnpackedVersion(const QString& packageName) const
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
int PackagesListModel::rowCount(const QModelIndex&) const
{
  return m_packageDetails.size();
}

/*!
  \brief Returns the data stored under \a role at \a index in the model.

  The role should make use of the \l PackageRoles enum.
 */
QVariant PackagesListModel::data(const QModelIndex& index, int role) const
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
  case DocumentsRole:
    return it.value().m_documentNames;
  case RequiresUnpackRole:
    return it.value().m_requiresUnpack;
  case UnpackedNameRole:
    return it.value().m_unpackedName;
  default:
    break;
  }

  return QVariant();
}

/*!
  \brief Returns the hash of role names used by the model.

  The roles are based on the \l DataItemRoles enum.
 */
QHash<int, QByteArray> PackagesListModel::roleNames() const
{
  return m_roles;
}

} // Dsa
