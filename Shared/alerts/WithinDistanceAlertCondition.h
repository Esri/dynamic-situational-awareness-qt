// Copyright 2017 ESRI
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

#ifndef WITHINDISTANCEALERTCONDITION_H
#define WITHINDISTANCEALERTCONDITION_H

#include "AlertCondition.h"

#include <QObject>

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
class GraphicsOverlay;
}
}

class AbstractOverlayManager;
class AlertSource;

class WithinDistanceAlertCondition : public AlertCondition
{
  Q_OBJECT

public:
  explicit WithinDistanceAlertCondition(AlertSource* source,
                                        Esri::ArcGISRuntime::GeoElement* target,
                                        double distance,
                                        AlertLevel level,
                                        const QString& name,
                                        QObject* parent = nullptr);

  explicit WithinDistanceAlertCondition(Esri::ArcGISRuntime::GraphicsOverlay* source,
                                        Esri::ArcGISRuntime::GeoElement* target,
                                        double distance,
                                        AlertLevel level,
                                        const QString& name,
                                        QObject* parent = nullptr);

  explicit WithinDistanceAlertCondition(Esri::ArcGISRuntime::GraphicsOverlay* source,
                                        AbstractOverlayManager* targetOverlay,
                                        double distance,
                                        AlertLevel level,
                                        const QString& name,
                                        QObject* parent = nullptr);

  ~WithinDistanceAlertCondition();

private:
  void connectSourceSignals(Esri::ArcGISRuntime::GraphicsOverlay* sourceOverlay);
  void connectTargetOverlaySignals();

  Esri::ArcGISRuntime::GeoElement* m_target = nullptr;
  AbstractOverlayManager* m_targetOverlay = nullptr;
  double m_distance;
};

#endif // WITHINDISTANCEALERTCONDITION_H
