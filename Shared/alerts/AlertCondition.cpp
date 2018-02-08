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
#include "GraphicAlertSource.h"

#include "GraphicsOverlay.h"
#include "GraphicListModel.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class AlertCondition
  \inherits QObject
  \brief Represents a condition which will be coninuosly monitored and will
  trigger an alert when met.

  An AlertCondition is made up of an \l AlertSource (generally some form of real-time feed)
  a query and an \l AlertTarget (a real time feed or an overlay).

  The condition is applied to all source objects, creating an \l AlertConditionData for each.

  When either the source or target is changed for a given data element, the condition can be
  re-tested using an \l AlertQuery to determine whether an elert should be triggered.

  \note This is an abstract base type.

  \sa AlertSource
  \sa AlertTarget
  \sa AlertConditionData
  */

/*!
  \brief Constructor taking an \l AlertLevel (\a level) the \a name of the condition and
  an optional \a parent.
 */
AlertCondition::AlertCondition(const AlertLevel& level,
                               const QString& name,
                               QObject* parent):
  QObject(parent),
  m_level(level),
  m_name(name)
{

}

/*!
  \brief Initializes the condition with a \a source and \a target with a \a sourceDescription and a \a targetDescription.

  A new \a AlertConditionData will be created to track changes to the
  source and target.
 */
void AlertCondition::init(AlertSource* source, AlertTarget* target, const QString& sourceDescription, const QString& targetDescription)
{
  if (!source || !target)
    return;

  m_sourceDescription = sourceDescription;
  m_targetDescription = targetDescription;
  AlertConditionData* newData = createData(source, target);
  addData(newData);
}

/*!
  \brief Initializes the condition with a \a sourceFeed and \a target and a \a targetDescription.

  A new \a AlertConditionData will be created for each \l Esri::ArcGISRuntime::Graphic
  in the source feed, to track changes to the source and target.
 */
void AlertCondition::init(GraphicsOverlay* sourceFeed, AlertTarget* target, const QString& targetDescription)
{
  if (!sourceFeed || !target)
    return;

  m_sourceDescription = sourceFeed->overlayId();
  m_targetDescription = targetDescription;

  GraphicListModel* graphics = sourceFeed->graphics();
  if (!graphics)
    return;

  // process a new graphic from the source feed to create a new AlertConditionData
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

  // connect to the graphicAdded to add condition data for any new graphics
  connect(graphics, &GraphicListModel::graphicAdded, this, handleGraphicAt);

  // add condition data for all of the graphics which are in the overlay to begin with
  const int count = graphics->rowCount();
  for (int i = 0; i < count; ++i)
    handleGraphicAt(i);
}

/*!
  \brief Destructor.
 */
AlertCondition::~AlertCondition()
{
  emit noLongerValid();
}

/*!
  \brief Returns the \l AlertLevel of the condition.
 */
AlertLevel AlertCondition::level() const
{
  return m_level;
}

/*!
  \brief Sets the \l AlertLevel for the condition to \a level.
 */
void AlertCondition::setLevel(AlertLevel level)
{
  if (level == m_level)
    return;

  m_level = level;

  for (auto it = m_data.begin(); it != m_data.end(); ++it)
  {
    AlertConditionData* data = *it;
    if (data)
      data->setLevel(m_level);
  }

  emit conditionChanged();
}

/*!
  \brief Returns the name of the condition.
 */
QString AlertCondition::name() const
{
  return m_name;
}

/*!
  \brief Sets the name for the condition and associated data to \a name.
 */
void AlertCondition::setName(const QString& name)
{
  if (name == m_name)
    return;

  m_name = name;

  for (int i = 0; i < m_data.count(); ++i)
  {
    AlertConditionData* data = m_data.at(i);
    if (data)
      data->setName(m_name + QString(" (%1)").arg(QString::number(i)));
  }

  emit conditionChanged();
}

/*!
  \brief Returns a name for the next new condition data.

  \note This will be of the form "My Condition (1)".
 */
QString AlertCondition::newConditionDataName() const
{
  return m_name + QString(" (%1)").arg(QString::number(m_data.count()));
}

/*!
  \brief Appends \a newData to the list of data being tracked for this condition.
 */
void AlertCondition::addData(AlertConditionData* newData)
{
  if (!newData)
    return;

  m_data.append(newData);
  emit newConditionData(newData);
}

/*!
  \brief Returns the name of the condition source.
 */
QString AlertCondition::sourceDescription() const
{
  return m_sourceDescription;
}

/*!
  \brief Returns the name of the condition target.
 */
QString AlertCondition::targetDescription() const
{
  return m_targetDescription;
}

/*!
  \brief Returns the description of this condition target.
 */
QString AlertCondition::description() const
{
  return QString("%1 %2 %3").arg(sourceDescription(), queryString(), targetDescription());
}

/*!
  \brief Returns whether this condition is enabled.

  When enabled is \false the condition will not be checked and no alerts will be raised.
 */
bool AlertCondition::isConditionEnabled() const
{
  return m_enabled;
}

/*!
  \brief Sets this condition to be \a enabled.

  When enabled is \false the condition will not be checked and no alerts will be raised.
 */
void AlertCondition::setConditionEnabled(bool enabled)
{
  if (enabled == m_enabled)
    return;

  for (auto it = m_data.cbegin(); it != m_data.cend(); ++it)
  {
    AlertConditionData* data = *it;
    if (data)
      data->setConditionEnabled(enabled);
  }

  m_enabled = enabled;
  emit conditionEnabledChanged();
}
