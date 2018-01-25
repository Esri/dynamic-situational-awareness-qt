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

#include "GraphicAlertSource.h"
#include "WithinDistanceAlertCondition.h"
#include "WithinDistanceAlertConditionData.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

WithinDistanceAlertCondition::WithinDistanceAlertCondition(AlertSource* source,
                                                           GeoElement* target,
                                                           double distance,
                                                           const AlertLevel& level,
                                                           const QString& name,
                                                           QObject* parent):
  AlertCondition(level, name, parent),
  m_target(target)
{
  WithinDistanceAlertConditionData* data = new WithinDistanceAlertConditionData(this, source, m_target, distance);
  addData(data);
}

WithinDistanceAlertCondition::WithinDistanceAlertCondition(GraphicsOverlay* source,
                                                           GeoElement* target,
                                                           double distance,
                                                           const AlertLevel& level,
                                                           const QString& name,
                                                           QObject* parent):
  AlertCondition(level, name, parent),
  m_target(target),
  m_distance(distance)
{
  connectSourceSignals(source);
}

WithinDistanceAlertCondition::WithinDistanceAlertCondition(GraphicsOverlay* source,
                                                           AbstractOverlayManager* targetOverlay,
                                                           double distance,
                                                           const AlertLevel& level,
                                                           const QString& name,
                                                           QObject* parent):
  AlertCondition(level, name, parent),
  m_targetOverlay(targetOverlay),
  m_distance(distance)
{
  connectSourceSignals(source);
  connectTargetOverlaySignals();
}

WithinDistanceAlertCondition::~WithinDistanceAlertCondition()
{

}

void WithinDistanceAlertCondition::connectSourceSignals(GraphicsOverlay* sourceOverlay)
{
  if (!sourceOverlay)
    return;

  GraphicListModel* graphics = sourceOverlay->graphics();
  if (!graphics)
    return;

  auto handleGraphicAt = [this, graphics](int index)
  {
    if (!graphics)
      return;

    Graphic* newGraphic = graphics->at(index);
    if (!newGraphic)
      return;

    if (m_target)
    {
      GraphicAlertSource* source = new GraphicAlertSource(newGraphic);
      WithinDistanceAlertConditionData* data = new WithinDistanceAlertConditionData(this, source, m_target, m_distance);
      addData(data);
    }
    else if (m_targetOverlay)
    {
      // TODO
    }
  };

  connect(graphics, &GraphicListModel::graphicAdded, this, handleGraphicAt);

  const int count = graphics->rowCount();
  for (int i = 0; i < count; ++i)
    handleGraphicAt(i);
}

void WithinDistanceAlertCondition::connectTargetOverlaySignals()
{
  // TODO
}
