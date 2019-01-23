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

#ifndef GEOELEMENTUTILS_H
#define GEOELEMENTUTILS_H

// Qt headers
#include <QList>
#include <QObject>

namespace Esri {
namespace ArcGISRuntime {
  class GeoElement;
}
}

namespace Dsa {

class GeoElementSignaler : public QObject
{
  Q_OBJECT

public:
  GeoElementSignaler(Esri::ArcGISRuntime::GeoElement* geoElement, QObject* parent = nullptr);
  ~GeoElementSignaler();

  Esri::ArcGISRuntime::GeoElement* geoElement() const;

signals:
  void geometryChanged();

private:
  Esri::ArcGISRuntime::GeoElement* m_geoElement = nullptr;
};

namespace GeoElementUtils
{
  void setParent(const QList<Esri::ArcGISRuntime::GeoElement*>& geoElements, QObject* parent);
  void setParent(Esri::ArcGISRuntime::GeoElement* geoElement, QObject* parent);
  QObject* toQObject(Esri::ArcGISRuntime::GeoElement* geoElement);
}

} // Dsa

#endif // GEOELEMENTUTILS_H
