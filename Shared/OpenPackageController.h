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
#include <QObject>
#include <QStringList>

namespace Esri {
namespace ArcGISRuntime {
  class MobileScenePackage;
  class Scene;
}
}

namespace Dsa {

class OpenPackageController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QStringList packageNames READ packageNames WRITE setPackageNames NOTIFY packageNamesChanged)

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

public slots:

signals:
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  void packageDataPathChanged();
  void currentPackageNameChanged();
  void packageIndexChanged();
  void packageNamesChanged();

private:
  QString packageDataPath() const;
  bool setPackageDataPath(QString dataPath);
  QString currentPackageName() const;
  bool setCurrentPackageName(QString packageName);

  int packageIndex() const;
  bool setPackageIndex(int packageIndex);

  QStringList packageNames() const;
  void setPackageNames(QStringList packageNames);

  void loadMobileScenePackage(const QString& mspkPath);

  QString combinedPackagePath() const;

  QString m_packageDataPath;
  QString m_currentPackageName;
  int m_packageIndex = 0;

  QStringList m_packageNames;

  Esri::ArcGISRuntime::MobileScenePackage* m_mspk = nullptr;
};

} // Dsa

#endif // OPENPACKAGECONTROLLER_H
