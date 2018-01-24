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

#include "WithinAreaAlertCondition.h"
#include "WithinAreaAlertConditionData.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

WithinAreaAlertCondition::WithinAreaAlertCondition(GraphicsOverlay* source,
                                                   GeoElement* target,
                                                   const AlertLevel& level,
                                                   const QString& name,
                                                   QObject* parent):
  AlertCondition(level, name, parent),
  m_source(source),
  m_target(target)
{
  connectSourceSignals();
}

WithinAreaAlertCondition::WithinAreaAlertCondition(GraphicsOverlay* source,
                                                   AbstractOverlayManager* targetOverlay,
                                                   const AlertLevel& level,
                                                   const QString& name,
                                                   QObject* parent):
  AlertCondition(level, name, parent),
  m_source(source),
  m_targetOverlay(targetOverlay)
{
  connectSourceSignals();
  connectTargetOverlaySignals();
}

WithinAreaAlertCondition::~WithinAreaAlertCondition()
{

}

void WithinAreaAlertCondition::connectSourceSignals()
{
  GraphicListModel* graphics = m_source->graphics();
  if (!graphics)
    return;

  connect(graphics, &GraphicListModel::graphicAdded, this, &WithinAreaAlertCondition::handleGraphicAt);

  const int count = graphics->rowCount();
  for (int i = 0; i < count; ++i)
    handleGraphicAt(i);
}

void WithinAreaAlertCondition::connectTargetOverlaySignals()
{
  // TODO
}

void WithinAreaAlertCondition::handleGraphicAt(int index)
{
  GraphicListModel* graphics = m_source->graphics();
  if (!graphics)
    return;

  Graphic* newGraphic = graphics->at(index);
  if (!newGraphic)
    return;

  if (m_target)
  {
    WithinAreaAlertConditionData* data = new WithinAreaAlertConditionData(newGraphic, m_target, this);
    addData(data);
  }
  else if (m_targetOverlay)
  {
    // TODO
  }
}
