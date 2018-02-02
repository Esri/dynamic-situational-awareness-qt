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
#include "AlertSource.h"
#include "AlertTarget.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class AlertConditionData
  \inherits QObject
  \brief Represents the data to be tested as part of a condition.

  An AlertCondition is made up of an \l AlertSource (generally some form of real-time feed)
  a query and an \l AlertTarget (a real time feed or an overlay).

  The condition is applied to all source objects, creating an \l AlertConditionData for each.

  Each condition data can be thought of as an individual row in a table: to perform a condition
  query, a condition data must be created and tested for each object in the source.

  When either the source or target is changed for a given data element, the condition can be
  re-tested using an \l AlertQuery to determine whether an alert should be triggered.

  \note This is an abstract base type.

  \sa AlertSource
  \sa AlertTarget
  \sa AlertCondition
  */

/*!
  \brief Constructor for a new condition data object.

  \list
    \li \a name. The name of the condition.
    \li \a level. The \l AlertLevel for the condition.
    \li \a source. The source data for the condition (for example
      \l Esri::ArcGISRuntime::Graphic or a location).
    \li \a target. The target data for the condition. For example,
      a real-time feed or an overlay or a fixed value.
    \li \a parent. The (optional) parent object.
  \endlist
 */
AlertConditionData::AlertConditionData(const QString& name,
                                       AlertLevel level,
                                       AlertSource* source,
                                       AlertTarget* target,
                                       QObject* parent):
  QObject(parent),
  m_name(name),
  m_level(level),
  m_source(source),
  m_target(target)
{
  connect(m_source, &AlertSource::noLongerValid, this, &AlertConditionData::noLongerValid);
  connect(m_source, &AlertSource::dataChanged, this, [this]()
  {
    m_queryOutOfDate = true;
    emit dataChanged();
  });
  connect(m_source, &AlertSource::destroyed, this, [this]()
  {
    m_source = nullptr;
    emit noLongerValid();
  });
  connect(m_target, &AlertTarget::dataChanged, this, , [this]()
  {
    m_queryOutOfDate = true;
    emit dataChanged();
  });
  connect(m_target, &AlertTarget::destroyed, this, [this]()
  {
    m_target = nullptr;
    emit noLongerValid();
  });
}

/*!
  \brief Destructor.
 */
AlertConditionData::~AlertConditionData()
{
  emit noLongerValid();
}

/*!
  \brief Returns the \l AlertLevel of the condition data.
 */
AlertLevel AlertConditionData::level() const
{
  return m_level;
}

/*!
  \brief Sets the \l AlertLevel for this condition data to \a level.
 */
void AlertConditionData::setLevel(AlertLevel& level)
{
  if (level == m_level)
    return;

  m_level = level;
  emit dataChanged();
}

/*!
  \brief Returns the current location of the source object for this
  condition data.
 */
Point AlertConditionData::sourceLocation() const
{
  return m_source->location();
}

/*!
  \brief Sets the highlight state for the source object to \a highlighted.
 */
void AlertConditionData::highlight(bool highlighted)
{
  source()->setSelected(highlighted);
}

/*!
  \brief Returns the nanme of this condition data.
 */
QString AlertConditionData::name() const
{
  return m_name;
}

/*!
  \brief Sets the name of this condition data to \l name.
 */
void AlertConditionData::setName(const QString& name)
{
  if (name == m_name)
    return;

  m_name = name;
  emit dataChanged();
}

/*!
  \brief Returns the unique ID of this condition data.
 */
QUuid AlertConditionData::id() const
{
  return m_id;
}

/*!
  \brief Sets the ID of this condition data to \l id.
 */
void AlertConditionData::setId(const QUuid& id)
{
  if (m_id == id)
    return;

  m_id = id;
  emit dataChanged();
}

/*!
  \brief Returns the viewed state for this condition data.

  Whenever a condition data changes to become active, it will return
  to the unviewed state s that the user can be notified.
 */
bool AlertConditionData::viewed() const
{
  return m_viewed;
}

/*!
  \brief Sets the viewed state for this condition data to \a viewed.

  \sa viewedChanged
 */
void AlertConditionData::setViewed(bool viewed)
{
  if (viewed == m_viewed)
    return;

  m_viewed = viewed;
  emit viewedChanged();
}

/*!
  \brief Sets the active state for this condition data to \a active.

  \sa activeChanged
 */
void AlertConditionData::setActive(bool active)
{
  if (active == m_active)
    return;

  m_active = active;
  emit activeChanged();
}

/*!
  \brief Returns the \l AlertSource object for this condition data.

  A source object is generally a real-time feed, for example:

  \list
    \li a dynamic \l Esri::ArcGISRuntime::GraphicsOverlay
    \li the device's current position
  \endlist
 */
AlertSource* AlertConditionData::source() const
{
  return m_source;
}

/*!
  \brief Returns the \l AlertTarget object for this condition data.

  A target object can be an overlay, a real-time feed or fixed data.
 */
AlertTarget* AlertConditionData::target() const
{
  return m_target;
}

void AlertConditionData::cachedQueryResult() const
{
  return m_cachedQueryResult;
}

void AlertConditionData::setCachedQueryResult(bool result)
{
  m_cachedQueryResult = result;
}

bool AlertConditionData::queryOutOfDate() const
{
  return m_queryOutOfDate;
}

/*!
  \brief Returns the active state of this conditiom data.

  Should be \c true when the condition data is met.
 */
bool AlertConditionData::active() const
{
  return m_active;
}
