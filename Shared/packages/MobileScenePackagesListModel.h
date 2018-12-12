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

#ifndef MOBILESCENEPACKAGES_LISTMODEL_H
#define MOBILESCENEPACKAGES_LISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QHash>
#include <QMap>
#include <QStringList>

namespace Dsa {

class MobileScenePackagesListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum PackageRoles
  {
    PackageNameRole = Qt::UserRole + 1,
    ImageReadyRole = Qt::UserRole + 2,
    SceneNamesRole = Qt::UserRole + 3,
    RequiresUnpackRole = Qt::UserRole + 4,
    UnpackedNameRole = Qt::UserRole + 5,
    SceneImagesReadyRole = Qt::UserRole + 6,
    PackageTitleRole = Qt::UserRole + 7,
    PackageDescriptionRole = Qt::UserRole + 8
  };

  MobileScenePackagesListModel(QObject* parent = nullptr);
  ~MobileScenePackagesListModel();

  void addPackageData(const QString& packageName);
  void removePackageDetails(const QString& packageName);

  void setRequiresUnpack(const QString& packageName, bool requiresUnpack);
  void setImageReady(const QString& packageName, bool imageReady);
  void setSceneNames(const QString& packageName, const QStringList& sceneNames);
  void setUnpackedName(const QString& packageName, const QString& unpackedName);
  void setSceneImagesReady(const QString& packageName, bool sceneImagesReady);
  void setTitleAndDescription(const QString& packageName, const QString& title, const QString& description);

  bool isUnpackedVersion(const QString& packageName) const;

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:

  struct PackageDetails
  {
    QString m_unpackedName;
    bool m_requiresUnpack = false;
    bool m_imageReady = false;
    QStringList m_sceneNames;
    bool m_sceneImagesReady = false;
    QString m_title;
    QString m_description;
  };

  void broadcastDataChanged(const QMap<QString, PackageDetails>::iterator& changedIterator);

  QHash<int, QByteArray> m_roles;
  QMap<QString, PackageDetails> m_packageDetails;
};

} // Dsa

#endif // MOBILESCENEPACKAGES_LISTMODEL_H
