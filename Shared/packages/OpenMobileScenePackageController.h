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

#ifndef OPENPACKAGECONTROLLER_H
#define OPENPACKAGECONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>
#include <QHash>
#include <QObject>
#include <QStringList>

class QImage;

namespace Esri {
namespace ArcGISRuntime {
class MobileScenePackage;
class Scene;
}
}

namespace Dsa {

class PackagesListModel;

class OpenPackageController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* packages READ packages NOTIFY packagesChanged)
  Q_PROPERTY(QString currentPackageName READ currentPackageName NOTIFY currentPackageNameChanged)
  Q_PROPERTY(int currentDocumentIndex READ currentDocumentIndex NOTIFY currentPackageNameChanged)

public:
  static const QString PACKAGE_DIRECTORY_PROPERTYNAME;
  static const QString CURRENT_PACKAGE_PROPERTYNAME;
  static const QString PACKAGE_INDEX_PROPERTYNAME;
  static const QString MSPK_EXTENSION;
  static const QString MMPK_EXTENSION;

  explicit OpenPackageController(QObject* parent = nullptr);
  ~OpenPackageController();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  void findPackage();
  void loadGeoDocument();

  Q_INVOKABLE void selectPackageName(QString newPackageName);
  Q_INVOKABLE void selectDocument(int newDocumentIndex);
  Q_INVOKABLE void unpack();

signals:
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  void packageDataPathChanged();
  void currentPackageNameChanged();
  void packageIndexChanged();
  void packageNamesChanged();
  void imageReady(const QString& packageName, const QImage& packageImage);
  void packagesChanged();
  void unpackCompleted(const QStringList& scenes);

private slots:
  void handleIsDirectReadSupportedCompleted(QUuid taskId, bool directReadSupported);

private:
  QString packageDataPath() const;
  bool setPackageDataPath(QString dataPath);
  QString currentPackageName() const;
  bool setCurrentPackageName(QString packageName);

  int currentDocumentIndex() const;
  bool setCurrentDocumentIndex(int currentDocumentIndex);

  void updatePackageDetails();

  void loadMobileScenePackage(const QString& packageName);
  bool createPackageDetails(const QString& packageName);

  QString combinedPackagePath() const;

  QAbstractListModel* packages() const;

  static QString getPackedName(const QString& packageName);
  static QString getUnpackedName(const QString& packageName);

  Esri::ArcGISRuntime::MobileScenePackage* getPackage(const QString& packageName);
  void loadCurrentGeoDocument(Esri::ArcGISRuntime::MobileScenePackage* package);

  QString m_packageDataPath;
  QString m_currentPackageName;
  int m_currentDocumentIndex = -1;
  PackagesListModel* m_packagesModel = nullptr;
  Esri::ArcGISRuntime::MobileScenePackage* m_mspk = nullptr;
  QHash<QUuid, QString> m_directReadTasks;
  QHash<QString, Esri::ArcGISRuntime::MobileScenePackage*> m_packages;
};

} // Dsa

#endif // OPENPACKAGECONTROLLER_H
