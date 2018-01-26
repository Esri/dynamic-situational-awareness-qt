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

#include "AlertCondition.h"
#include "AlertConditionData.h"
#include "AlertListModel.h"
#include "GraphicAlertSource.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

AlertCondition::AlertCondition(const AlertLevel& level,
                               const QString& name,
                               QObject* parent):
  QObject(parent),
  m_level(level),
  m_name(name)
{

}

void AlertCondition::init(AlertSource* source, AlertTarget* target)
{
  AlertConditionData* newData = createData(source, target);
  addData(newData);
}

void AlertCondition::init(GraphicsOverlay* sourceFeed, AlertTarget* target)
{
  if (!sourceFeed)
    return;

  GraphicListModel* graphics = sourceFeed->graphics();
  if (!graphics)
    return;

  auto handleGraphicAt = [this, graphics, target](int index)
  {
    if (!graphics)
      return;

    Graphic* newGraphic = graphics->at(index);
    if (!newGraphic)
      return;

    GraphicAlertSource* source = new GraphicAlertSource(newGraphic);
    AlertConditionData* newData = createData(source, target);
    addData(newData);
  };

  connect(graphics, &GraphicListModel::graphicAdded, this, handleGraphicAt);

  const int count = graphics->rowCount();
  for (int i = 0; i < count; ++i)
    handleGraphicAt(i);
}

AlertCondition::~AlertCondition()
{
  emit noLongerValid();
}

AlertLevel AlertCondition::level() const
{
  return m_level;
}

void AlertCondition::alertLevel(const AlertLevel& level)
{
  if (level == m_level)
    return;

  m_level = level;
}

QString AlertCondition::name() const
{
  return m_name;
}

void AlertCondition::setName(const QString& name)
{
  if (name == m_name)
    return;

  m_name = name;
}

void AlertCondition::addData(AlertConditionData* newData)
{
  if (!newData)
    return;

  AlertListModel::instance()->addAlertConditionData(newData);
}

