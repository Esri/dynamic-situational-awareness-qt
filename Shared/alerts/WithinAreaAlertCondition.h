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

#ifndef WITHINAREAALERTCONDITION_H
#define WITHINAREAALERTCONDITION_H

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

class WithinAreaAlertCondition : public AlertCondition
{
  Q_OBJECT

public:
  explicit WithinAreaAlertCondition(Esri::ArcGISRuntime::GraphicsOverlay* source,
                                        Esri::ArcGISRuntime::GeoElement* target,
                                        const AlertLevel& level,
                                        const QString& name,
                                        QObject* parent = nullptr);

  explicit WithinAreaAlertCondition(Esri::ArcGISRuntime::GraphicsOverlay* source,
                                        AbstractOverlayManager* targetOverlay,
                                        const AlertLevel& level,
                                        const QString& name,
                                        QObject* parent = nullptr);

  ~WithinAreaAlertCondition();

private:
  void connectSourceSignals();
  void connectTargetOverlaySignals();
  void handleGraphicAt(int index);

  Esri::ArcGISRuntime::GraphicsOverlay* m_source = nullptr;
  Esri::ArcGISRuntime::GeoElement* m_target = nullptr;
  AbstractOverlayManager* m_targetOverlay = nullptr;
};

#endif // WITHINAREAALERTCONDITION_H
