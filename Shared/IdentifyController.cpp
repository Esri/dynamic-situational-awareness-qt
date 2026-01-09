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

#include "IdentifyController.h"

// C++ API headers
#include "AttributeListModel.h"
#include "DynamicEntity.h"
#include "DynamicEntityChangedInfo.h"
#include "DynamicEntityObservation.h"
#include "FieldsPopupElement.h"
#include "GeoElement.h"
#include "GeoView.h"
#include "Popup.h"
#include "PopupDefinition.h"
#include "PopupElement.h"
#include "PopupField.h"
#include "PopupFieldFormat.h"
#include "PopupFieldListModel.h"
#include "PopupTypes.h"

// DSA headers
#include "ToolManager.h"

// STL headers
#include <iterator>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

IdentifyController::IdentifyController(QObject* parent /* = nullptr */):
  AbstractTool(parent)
{
  ToolManager::instance().addTool(this);
}

IdentifyController::~IdentifyController() = default;

QString IdentifyController::toolName() const
{
  return QStringLiteral("identify");
}

void IdentifyController::setGeoElements(const std::vector<ContextMenu::Element>& contextElements)
{
  reset();

  if (contextElements.empty())
  {
    emit popupChanged();
    return;
  }

  m_contextElements.reserve(contextElements.size());
  std::for_each(std::cbegin(contextElements), std::cend(contextElements), [&](const ContextMenu::Element& ce)
  {
    GeoElement* ge = std::get<GeoElement*>(ce);
    if (!ge)
      return;

    // dynamic entities do not need to be re-parented but a weak pointer should be captured for them
    const ContextMenu::DynamicEntityPtr dePtr = std::get<ContextMenu::DynamicEntityPtr>(ce);
    if (dePtr.has_value() && dePtr->isNull())
        return;
    else
    {
      // if no popup can be created for the element because it has no
      // attributes, it should be released and skipped
      QObject* o = dynamic_cast<QObject*>(ge);
      if (!ge->attributes() || ge->attributes()->isEmpty())
      {
        o->deleteLater();
        return;
      }

      o->setParent(this);
    }

    m_contextElements.emplace_back(ce);
  });

  // abort if nothing could be added due to missing attributes
  // or expired dynamic entity pointers
  if (m_contextElements.empty())
  {
    emit popupChanged();
    return;
  }

  nextPopup();
}

void IdentifyController::nextPopup()
{
  if (!canNext())
    return;

  ++m_geoElementIndex;
  setPopup();
}

void IdentifyController::prevPopup()
{
  if (!canPrev())
    return;

  --m_geoElementIndex;
  setPopup();
}

void IdentifyController::reset()
{
  if (m_dynamicEntityConnection)
    disconnect(m_dynamicEntityConnection);

  m_geoElementIndex = -1;
  if (m_popup)
  {
    m_popup->deleteLater();
    delete m_popup;
  }
  m_popup = nullptr;

  std::for_each(std::cbegin(m_contextElements), std::cend(m_contextElements), [&](const ContextMenu::Element& ce)
  {
    const ContextMenu::DynamicEntityPtr dePtr = std::get<ContextMenu::DynamicEntityPtr>(ce);
    if (dePtr.has_value())
      return;

    QObject* o = dynamic_cast<QObject*>(std::get<GeoElement*>(ce));
    o->deleteLater();
    delete o;
  });

  m_contextElements.clear();
}

bool isObjectIdFieldName(QStringView fieldName)
{
  static constexpr std::array<std::string_view, 3> oidFieldNames{
    "OBJECTID",
    "FID",
    "OID",
  };
  static constexpr auto cbegin = std::cbegin(oidFieldNames);
  static constexpr auto cend = std::cend(oidFieldNames);

  return std::any_of(cbegin, cend, [&](auto oidFieldName) { return fieldName.compare(oidFieldName, Qt::CaseInsensitive) == 0; });
}

void IdentifyController::setPopup()
{
  if (m_dynamicEntityConnection)
    disconnect(m_dynamicEntityConnection);

  const ContextMenu::Element ce = m_contextElements[m_geoElementIndex];
  GeoElement* ge = std::get<GeoElement*>(ce);
  const QString popupTitle = std::get<QString>(ce);
  const ContextMenu::DynamicEntityPtr dePtr = std::get<ContextMenu::DynamicEntityPtr>(ce);

  if (dePtr.has_value() && dePtr->isNull())
  {
    std::vector<ContextMenu::Element> validElements{};
    validElements.reserve(m_contextElements.size() - 1); // we know we will at least remove 1
    std::copy_if(std::cbegin(m_contextElements), std::cend(m_contextElements), std::back_inserter(validElements), [&](const ContextMenu::Element& ce)
    {
      const ContextMenu::DynamicEntityPtr dePtr = std::get<ContextMenu::DynamicEntityPtr>(ce);
      return !dePtr.has_value() || !dePtr->isNull();
    });
    resetGeoElements(validElements);
    return;
  }

  // create a new Popup from the geoElement
  Popup* newPopup = new Popup(ge, this);
  newPopup->popupDefinition()->setTitle(popupTitle);
  const auto popupElements = newPopup->popupDefinition()->elements();
  for (const PopupElement* popupElement : popupElements)
  {
    if (popupElement->popupElementType() != PopupElementType::FieldsPopupElement)
      continue;

    const auto* fieldsPE = static_cast<const FieldsPopupElement*>(popupElement);
    const auto* fields = fieldsPE->fields();
    for (PopupField* field : *fields)
    {
      // check any fields that are not editable for common ObjectID field names
      if (!field->isEditable() && isObjectIdFieldName(field->fieldName()))
        continue;

      // set parent to Popup
      auto popupFF = new PopupFieldFormat(dynamic_cast<QObject*>(newPopup));
      popupFF->setDecimalPlaces(2);
      popupFF->setUseThousandsSeparator(true);
      field->setFormat(popupFF);
    }
  }

  if (dePtr.has_value() && !dePtr->isNull())
    m_dynamicEntityConnection = connect(dePtr.value(), &DynamicEntity::dynamicEntityChanged, this, &IdentifyController::dynamicEntityChanged);

  if (m_popup)
  {
    m_popup->deleteLater();
    delete m_popup;
  }

  m_popup = newPopup;
  emit popupChanged();
}

void IdentifyController::resetGeoElements(const std::vector<ContextMenu::Element> &contextElements)
{
  if (m_dynamicEntityConnection)
    disconnect(m_dynamicEntityConnection);

  m_geoElementIndex = -1;
  if (m_popup)
  {
    m_popup->deleteLater();
    delete m_popup;
  }
  m_popup = nullptr;

  m_contextElements.clear();

  if (contextElements.empty())
  {
    emit popupChanged();
    return;
  }

  m_contextElements.reserve(contextElements.size());
  std::for_each(std::begin(contextElements), std::end(contextElements), [&](const ContextMenu::Element& ce)
  {
    m_contextElements.emplace_back(ce);
  });

  nextPopup();
}

bool IdentifyController::canNext() const
{
  return m_geoElementIndex < static_cast<int>(m_contextElements.size() - 1);
}

bool IdentifyController::canPrev() const
{
  return m_geoElementIndex > 0;
}

void IdentifyController::dynamicEntityChanged(Esri::ArcGISRuntime::DynamicEntityChangedInfo* deci)
{
  deci->deleteLater();

  // if the entity has been purged, break the connection and skip setting the popup
  // this leaves the UI populated with the latest observation info until navigating
  // to another element with back/next buttons
  if (deci->isDynamicEntityPurged())
    disconnect(m_dynamicEntityConnection);
  else
    setPopup();
}

Popup* IdentifyController::popup() const
{
  if (!m_popup || m_contextElements.size() < 1)
    return nullptr;

  return m_popup;
}

} // Dsa
