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

#ifndef OPENMOBILESCENEPACKAGECONTROLLER_H
#define OPENMOBILESCENEPACKAGECONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>
#include <QHash>
#include <QObject>
#include <QStringList>
#include <QUuid>

class QImage;

namespace Esri {
namespace ArcGISRuntime {
class MobileScenePackage;
class Scene;
}
}

namespace Dsa {

class MobileScenePackagesListModel;

class OpenMobileScenePackageController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractListModel* packages READ packages NOTIFY packagesChanged)
  Q_PROPERTY(QString currentPackageName READ currentPackageName NOTIFY currentSceneNameChanged)
  Q_PROPERTY(int currentSceneIndex READ currentSceneIndex NOTIFY currentSceneNameChanged)

public:
  static const QString PACKAGE_DIRECTORY_PROPERTYNAME;
  static const QString CURRENT_PACKAGE_PROPERTYNAME;
  static const QString SCENE_INDEX_PROPERTYNAME;

  explicit OpenMobileScenePackageController(QObject* parent = nullptr);
  ~OpenMobileScenePackageController();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

  Q_INVOKABLE void selectPackageName(QString newPackageName);
  Q_INVOKABLE void selectScene(int newSceneIndex);
  Q_INVOKABLE void unpack();

  QString packageDataPath() const;
  bool setPackageDataPath(QString dataPath);

  QString currentPackageName() const;
  bool setCurrentPackageName(QString packageName);

  int currentSceneIndex() const;
  bool setCurrentSceneIndex(int currentSceneIndex);

  QAbstractListModel* packages() const;

signals:
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  void packageDataPathChanged();
  void currentSceneNameChanged();
  void packageIndexChanged();
  void imageReady(const QString& packageName, const QImage& packageImage);
  void packagesChanged();

private slots:
  void handleIsDirectReadSupportedCompleted(QUuid taskId, bool directReadSupported);

private:
  void findPackage();
  void loadScene();

  void updatePackageDetails();

  void loadMobileScenePackage(const QString& packageName);
  bool createPackageDetails(const QString& packageName);

  QString combinedPackagePath() const;

  static QString getPackedName(const QString& packageName);
  static QString getUnpackedName(const QString& packageName);
  QString pathInPackagesDirectory(const QString& packageName) const;

  Esri::ArcGISRuntime::MobileScenePackage* getPackage(const QString& packageName);
  void loadCurrentScene(Esri::ArcGISRuntime::MobileScenePackage* package);

  static const QString MSPK_EXTENSION;
  static const QString MMPK_EXTENSION;
  static const QString UNPACKED_SUFFIX;

  QString m_packageDataPath;
  QString m_currentPackageName;
  int m_currentSceneIndex = -1;
  MobileScenePackagesListModel* m_packagesModel = nullptr;
  Esri::ArcGISRuntime::MobileScenePackage* m_mspk = nullptr;
  QHash<QUuid, QString> m_directReadTasks;
  QHash<QString, Esri::ArcGISRuntime::MobileScenePackage*> m_packages;
};

} // Dsa

#endif // OPENMOBILESCENEPACKAGECONTROLLER_H
