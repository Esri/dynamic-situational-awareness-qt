/*******************************************************************************
 *  Copyright 2012-2024 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

#include "DynamicEntityAlertSource.h"

// C++ API headers
#include "AttributeListModel.h"
#include "DynamicEntity.h"
#include "DynamicEntityChangedInfo.h"
#include "DynamicEntityLayer.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::DynamicEntityAlertSource
  \inmodule Dsa
  \inherits AlertSource
  \brief Represents a source based on a single \l Esri::ArcGISRuntime::DynamicEntity
  for an \l AlertCondition.

  Observations to the underlying DynamicEntity will cause the \l AlertSource::dataChanged
  signal to be emitted.
 */

/*!
 * \brief Constructor taking a DynamicEntity and it's parent DynamicEntityLayer
 * \param dynamicEntity
 * \param dynamicEntityLayer
 */
DynamicEntityAlertSource::DynamicEntityAlertSource(DynamicEntity* dynamicEntity, DynamicEntityLayer* dynamicEntityLayer):
  AlertSource(dynamicEntity),
  m_dynamicEntity(dynamicEntity),
  m_dynamicEntityLayer(dynamicEntityLayer)
{
  connect(m_dynamicEntity, &DynamicEntity::dynamicEntityChanged, this, [this](DynamicEntityChangedInfo* info)
  {
    emit dataChanged();
    info->deleteLater();
  });
}

/*!
 * \brief DynamicEntityAlertSource::~DynamicEntityAlertSource
 */
DynamicEntityAlertSource::~DynamicEntityAlertSource()
{

}

/*!
  \brief Returns the location of the underlying \l Esri::ArcGISRuntime::DynamicEntity.
 */
Point DynamicEntityAlertSource::location() const
{
  return geometry_cast<Point>(m_dynamicEntity->geometry());
}

/*!
  \brief Returns the attribute value for the field \a key in the
  underlying \l Esri::ArcGISRuntime::DynamicEntity.
 */
QVariant DynamicEntityAlertSource::value(const QString& key) const
{
  if (!m_dynamicEntity->attributes())
    return QVariant();

  return m_dynamicEntity->attributes()->attributeValue(key);
}

/*!
  \brief Sets the selected state of the \l Esri::ArcGISRuntime::DynamicEntity to \a selected.
 */
void DynamicEntityAlertSource::setSelected(bool selected)
{
  if (selected)
    m_dynamicEntityLayer->selectDynamicEntity(m_dynamicEntity);
  else
    m_dynamicEntityLayer->unselectDynamicEntity(m_dynamicEntity);
}

} // Dsa
