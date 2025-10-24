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

#ifndef DSARESOURCES_H
#define DSARESOURCES_H

// Qt headers
#include <QObject>
#include <QUrl>

namespace Dsa {

class DsaResources : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString arcGISMapsSDKVersion READ arcGISMapsSDKVersion NOTIFY arcGISMapsSDKVersionChanged)
  Q_PROPERTY(QUrl iconAppLogo READ iconAppLogo CONSTANT)

public:
  DsaResources(QObject* parent = nullptr);
  void setArcGISMapsSDKVersion(const char* arcGISMapsSDKVersion);

signals:
  void arcGISMapsSDKVersionChanged();

private:
  QString m_arcGISMapsSDKVersion;
  QString arcGISMapsSDKVersion() const;
  QUrl iconAppLogo() const;
};

} // Dsa

#endif // DSARESOURCES_H
