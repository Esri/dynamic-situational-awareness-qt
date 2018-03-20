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

#include "pch.hpp"

#include "GraphicAlertSource.h"

#include "AttributeListModel.h"
#include "Envelope.h"
#include "Graphic.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class GraphicAlertSource
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
    return m_graphic->geometry();
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
