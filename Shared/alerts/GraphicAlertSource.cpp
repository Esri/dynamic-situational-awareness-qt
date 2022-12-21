/*******************************************************************************
 *  Copyright 2012-2018 Esri
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

#include "GraphicAlertSource.h"

// C++ API headers
#include "AttributeListModel.h"
#include "Envelope.h"
#include "Graphic.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::GraphicAlertSource
  \inmodule Dsa
  \inherits AlertSource
  \brief Represents a source based on a single \l Esri::ArcGISRuntime::Graphic
  for an \l AlertCondition.

  Changes to the underlying graphic's position will cause the \l AlertSource::locationChanged
  signal to be emitted.
 */

/*!
  \brief Constructor taking an \l Esri::ArcGISRuntime::Graphic \a graphic.
 */
GraphicAlertSource::GraphicAlertSource(Graphic* graphic):
  AlertSource(graphic),
  m_graphic(graphic)
{
  connect(m_graphic, &Graphic::geometryChanged, this, &GraphicAlertSource::dataChanged);
  connect(m_graphic->attributes(), &AttributeListModel::modelReset, this, &GraphicAlertSource::dataChanged);
  connect(m_graphic->attributes(), &AttributeListModel::dataChanged, this, &GraphicAlertSource::dataChanged);
}

/*!
  \brief Destructor.
 */
GraphicAlertSource::~GraphicAlertSource()
{

}

/*!
  \brief Returns the location of the underlying \l Esri::ArcGISRuntime::Graphic.
 */
Point GraphicAlertSource::location() const
{
  if (m_graphic->geometry().geometryType() == GeometryType::Point)
    return geometry_cast<Point>(m_graphic->geometry());
  else
    return m_graphic->geometry().extent().center();
}

/*!
  \brief Returns the attribute value for the field \a key in the
  underlying \l Esri::ArcGISRuntime::Graphic.
 */
QVariant GraphicAlertSource::value(const QString& key) const
{
  if (!m_graphic->attributes())
    return QVariant();

  return m_graphic->attributes()->attributeValue(key);
}

/*!
  \brief Sets the selected state of the \l Esri::ArcGISRuntime::Graphic to \a selected.
 */
void GraphicAlertSource::setSelected(bool selected)
{
  m_graphic->setSelected(selected);
}

} // Dsa
