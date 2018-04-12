// Copyright 2018 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef VEHICLE_H
#define VEHICLE_H

// Qt headers
#include <QQuickItem>

namespace Esri {
namespace ArcGISRuntime {
class SceneQuickView;
}
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

signals:
  void errorOccurred(const QString& message, const QString& additionalMessage);

private:

  Esri::ArcGISRuntime::SceneQuickView*    m_sceneView = nullptr;
  DsaController*                          m_controller = nullptr;
};

} // Vehicle
} // Dsa

#endif // VEHICLE_H
