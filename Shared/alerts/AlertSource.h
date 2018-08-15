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

#ifndef ALERTSOURCE_H
#define ALERTSOURCE_H

// C++ API headers
#include "Point.h"

// Qt headers
#include <QObject>
#include <QVariant>

namespace Dsa {

class AlertSource : public QObject
{
  Q_OBJECT

public:
  explicit AlertSource(QObject* parent = nullptr);
  ~AlertSource();

  virtual Esri::ArcGISRuntime::Point location() const = 0;
  virtual QVariant value(const QString& key) const = 0;

  virtual void setSelected(bool selected) = 0;

signals:
  void dataChanged();
  void noLongerValid();
};

} // Dsa

#endif // ALERTSOURCE_H
