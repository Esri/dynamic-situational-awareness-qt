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

#ifndef PACKAGEIMAGEPROVIDER_H
#define PACKAGEIMAGEPROVIDER_H

// Qt headers
#include <QHash>
#include <QImage>
#include <QMetaObject>
#include <QObject>
#include <QQuickImageProvider>

namespace Dsa {

class OpenMobileScenePackageController;

class PackageImageProvider : public QQuickImageProvider
{
public:

  PackageImageProvider();

  QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

  private:
    QHash<QString, QImage> m_packages;
    OpenMobileScenePackageController* m_packageController = nullptr;

    QMetaObject::Connection m_findToolConnection;

    QImage m_defaultImage;
};

} // Dsa

#endif // PACKAGEIMAGEPROVIDER_H
