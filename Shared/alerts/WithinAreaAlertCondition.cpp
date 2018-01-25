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
#include "WithinAreaAlertCondition.h"
#include "WithinAreaAlertConditionData.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

WithinAreaAlertCondition::WithinAreaAlertCondition(AlertSource* source,
                                                   GeoElement* target,
                                                   const AlertLevel& level,
                                                   const QString &name,
                                                   QObject* parent):
  AlertCondition(level, name, parent),
  m_target(target)
{
  WithinAreaAlertConditionData* data = new WithinAreaAlertConditionData(this, source, m_target);
  addData(data);
}

WithinAreaAlertCondition::WithinAreaAlertCondition(GraphicsOverlay* source,
                                                   GeoElement* target,
                                                   const AlertLevel& level,
                                                   const QString& name,
                                                   QObject* parent):
  AlertCondition(level, name, parent),
  m_target(target)
{
  connectSourceSignals(source);
}

WithinAreaAlertCondition::WithinAreaAlertCondition(GraphicsOverlay* source,
                                                   AbstractOverlayManager* targetOverlay,
                                                   const AlertLevel& level,
                                                   const QString& name,
                                                   QObject* parent):
  AlertCondition(level, name, parent),
  m_targetOverlay(targetOverlay)
{
  connectSourceSignals(source);
  connectTargetOverlaySignals();
}

WithinAreaAlertCondition::~WithinAreaAlertCondition()
{

}

void WithinAreaAlertCondition::connectSourceSignals(GraphicsOverlay* sourceOverlay)
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
      WithinAreaAlertConditionData* data = new WithinAreaAlertConditionData(this, source, m_target);
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

void WithinAreaAlertCondition::connectTargetOverlaySignals()
{
  // TODO
}
