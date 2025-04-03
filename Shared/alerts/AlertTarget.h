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

#ifndef ALERTTARGET_H
#define ALERTTARGET_H

// Qt headers
#include <QObject>
#include <QVariant>

namespace Esri::ArcGISRuntime {
  class Envelope;
  class Geometry;
}

namespace Dsa {

class AlertTarget : public QObject
{
  Q_OBJECT

public:
  explicit AlertTarget(QObject* parent = nullptr);
  ~AlertTarget();

  virtual QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const = 0;
  virtual QVariant targetValue() const = 0;

signals:
  void noLongerValid();
  void dataChanged();
};

} // Dsa

#endif // ALERTTARGET_H
