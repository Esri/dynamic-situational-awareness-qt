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

#include "WithinDistanceAlertCondition.h"
#include "WithinDistanceAlertConditionData.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

WithinDistanceAlertCondition::WithinDistanceAlertCondition(GraphicsOverlay* source,
                                                           GeoElement* target,
                                                           double distance,
                                                           const AlertLevel& level,
                                                           const QString& name,
                                                           QObject* parent):
  AlertCondition(level, name, parent),
  m_source(source),
  m_target(target),
  m_distance(distance)
{
  connectSourceSignals();
}

WithinDistanceAlertCondition::WithinDistanceAlertCondition(GraphicsOverlay* source,
                                                           AbstractOverlayManager* targetOverlay,
                                                           double distance,
                                                           const AlertLevel& level,
                                                           const QString& name,
                                                           QObject* parent):
  AlertCondition(level, name, parent),
  m_source(source),
  m_targetOverlay(targetOverlay),
  m_distance(distance)
{
  connectSourceSignals();
  connectTargetOverlaySignals();
}

WithinDistanceAlertCondition::~WithinDistanceAlertCondition()
{

}

void WithinDistanceAlertCondition::connectSourceSignals()
{
  GraphicListModel* graphics = m_source->graphics();
  if (!graphics)
    return;

  connect(graphics, &GraphicListModel::graphicAdded, this, &WithinDistanceAlertCondition::handleGraphicAt);

  const int count = graphics->rowCount();
  for (int i = 0; i < count; ++i)
    handleGraphicAt(i);
}

void WithinDistanceAlertCondition::connectTargetOverlaySignals()
{
  // TODO
}

void WithinDistanceAlertCondition::handleGraphicAt(int index)
{
  GraphicListModel* graphics = m_source->graphics();
  if (!graphics)
    return;

  Graphic* newGraphic = graphics->at(index);
  if (!newGraphic)
    return;

  if (m_target)
  {
    WithinDistanceAlertConditionData* data = new WithinDistanceAlertConditionData(newGraphic, m_target, m_distance, this);
    addData(data);
  }
  else if (m_targetOverlay)
  {
    // TODO
  }
}
