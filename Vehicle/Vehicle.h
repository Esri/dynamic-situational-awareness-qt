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

#ifndef VEHICLE_H
#define VEHICLE_H

// Qt headers
#include <QQuickItem>

namespace Esri::ArcGISRuntime {
  class SceneQuickView;
}

namespace Dsa {

class DsaController;

namespace Vehicle {

class Vehicle : public QQuickItem
{
  Q_OBJECT

public:
  Vehicle(QQuickItem* parent = nullptr);
  ~Vehicle();

  void componentComplete() override;
  Q_INVOKABLE void resetToDefaultScene();

signals:
  void errorOccurred(const QString& message, const QString& additionalMessage);

private:

  Esri::ArcGISRuntime::SceneQuickView*    m_sceneView = nullptr;
  DsaController*                          m_controller = nullptr;
};

} // Vehicle
} // Dsa

#endif // VEHICLE_H
