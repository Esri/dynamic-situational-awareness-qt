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

#ifndef PACKAGES_LISTMODEL_H
#define PACKAGES_LISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QHash>
#include <QMap>
#include <QStringList>

namespace Dsa {

class PackagesListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum PackageRoles
  {
    PackageNameRole = Qt::UserRole + 1,
    ImageReadyRole = Qt::UserRole + 2,
    DocumentsRole = Qt::UserRole + 3,
    RequiresUnpackRole = Qt::UserRole + 4
  };

  PackagesListModel(QObject* parent = nullptr);
  ~PackagesListModel();

  void addPackageData(const QString& packageName);
  void removePackageDetails(const QString& packageName);

  void setRequiresUnpack(const QString& packageName, bool requiresUnpack);
  void setImageReady(const QString& packageName, bool imageReady);
  void setDocumentNames(const QString& packageName, QStringList documentNames);

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:

  struct PackageDetails
  {
    bool m_requiresUnpack = false;
    bool m_imageReady = false;
    QStringList m_documentNames;
  };

  QHash<int, QByteArray> m_roles;
  QMap<QString, PackageDetails> m_packageDetails;
};

} // Dsa

#endif // PACKAGES_LISTMODEL_H
