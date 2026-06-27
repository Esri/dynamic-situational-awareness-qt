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

// C++ API
#include "AttributeListModel.h"
#include "DynamicEntity.h"
#include "FieldsPopupElement.h"
#include "GeoElement.h"
#include "GeoView.h"
#include "Popup.h"
#include "PopupDefinition.h"
#include "PopupElement.h"
#include "PopupExpression.h"
#include "PopupField.h"
#include "PopupFieldFormat.h"
#include "PopupFieldListModel.h"
#include "PopupTypes.h"
// DSA
#include "GeoElementUtils.h"
#include "MessageFeedConstants.h"
#include "ToolManager.h"
// Qt
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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

/*!
 * \brief Creates popups for the GeoElements and takes ownership of any non-DynamicEntities.
 */
void IdentifyController::showPopups(const QHash<QString, QList<GeoElement*>>& geoElementsByTitle)
{
  clearPopups();

  if (geoElementsByTitle.isEmpty())
  {
    emit popupChanged();
    return;
  }

  for (const auto& [title, geoElements] : geoElementsByTitle.asKeyValueRange())
  {
    for (auto* geoElement : geoElements)
      addGeoElementPopup(geoElement, title);
  }

  // verify at least one popup was added and set the pointer to the start
  if (!m_popups.empty())
    m_currentPopupIndex = 0;

  emit popupChanged();
}

void IdentifyController::nextPopup()
{
  if (!canNext())
    return;

  ++m_currentPopupIndex;
  emit popupChanged();
}

void IdentifyController::prevPopup()
{
  if (!canPrev())
    return;

  --m_currentPopupIndex;
  emit popupChanged();
}

void IdentifyController::clearPopups()
{
  for (Popup* popup : m_popups)
    popup->deleteLater();

  m_popups.clear();

  m_currentPopupIndex = -1;
}

QString getPopupDefinitionUrlForMessageType(const QString& messageType)
{
  const std::unordered_map<QString, QString>& urls = MessageFeeds::Popups::SCHEMA_URLS;
  if (urls.find(messageType) != urls.end())
  {
    return urls.at(messageType);
  }

  return QString{};
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

  return std::any_of(cbegin, cend, [fieldName](auto oidFieldName) { return fieldName.compare(oidFieldName, Qt::CaseInsensitive) == 0; });
}

bool IdentifyController::addGeoElementPopup(GeoElement* geoElement, const QString& popupTitle)
{
  if (!geoElement)
    return false;

  if (!geoElement->attributes() || geoElement->attributes()->isEmpty())
    return false;

  // check for dynamic entities
  if (const auto* de = dynamic_cast<DynamicEntity*>(geoElement); de)
  {
    Popup* newPopup = nullptr;
    if (const QVariant geoElementTypeV = geoElement->attributes()->attributeValue(MessageFeeds::Fields::GeoMessage::TYPE); geoElementTypeV.isValid())
    {
      if (const QString popupDefinitionUrl = getPopupDefinitionUrlForMessageType(geoElementTypeV.toString()); !popupDefinitionUrl.isEmpty())
      {
        if (PopupDefinition* popupDefinition = getPopupDefinitionForUrl(popupDefinitionUrl); popupDefinition)
          newPopup = new Popup(geoElement, popupDefinition, this);
      }
    }

    // default popup to be used for CoT types currently
    if (!newPopup)
      newPopup = new Popup(geoElement, this);

    newPopup->popupDefinition()->setTitle(QString{"%1<br>(Live Updates)"}.arg(popupTitle));
    m_popups.push_back(newPopup);
    return true;
  }

  // default popup title to the layer name
  auto* newPopup = new Popup(geoElement, this);
  newPopup->popupDefinition()->setTitle(popupTitle);
  GeoElementUtils::toQObject(geoElement)->setParent(newPopup);

  const QList<PopupElement*> popupElements = newPopup->popupDefinition()->elements();
  for (const PopupElement* popupElement : popupElements)
  {
    if (popupElement->popupElementType() != PopupElementType::FieldsPopupElement)
      continue;

    const auto* fieldsPE = static_cast<const FieldsPopupElement*>(popupElement);
    const PopupFieldListModel* fields = fieldsPE->fields();
    for (PopupField* field : *fields)
    {
      // check any fields that are not editable for common ObjectID field names
      if (!field->isEditable() && isObjectIdFieldName(field->fieldName()))
        continue;

      // set parent to Popup
      auto popupFF = new PopupFieldFormat(static_cast<QObject*>(newPopup));
      popupFF->setDecimalPlaces(2);
      popupFF->setUseThousandsSeparator(true);
      field->setFormat(popupFF);
    }
  }
  m_popups.push_back(newPopup);

  return true;
}

bool IdentifyController::canNext() const
{
  return m_currentPopupIndex < static_cast<int>(m_popups.size() - 1);
}

bool IdentifyController::canPrev() const
{
  return m_currentPopupIndex > 0;
}

PopupDefinition* IdentifyController::getPopupDefinitionForUrl(const QString& url)
{
  if (m_popupDefinitions.find(url) == m_popupDefinitions.cend())
  {
    qDebug() << "creating popup def for" << url;
    QFile fileGeoMessage{url};
    fileGeoMessage.open(QFile::ReadOnly);

    const QJsonObject popupInfo = QJsonDocument::fromJson(fileGeoMessage.readAll()).object()["popupInfo"].toObject();
    fileGeoMessage.close();
    QList<PopupElement*> popupElements{};
    for (const QJsonValue& v : popupInfo["popupElements"].toArray())
    {
      QJsonObject o{v.toObject()};
      PopupElement* pe = PopupElement::fromJson(QJsonDocument{o}.toJson(QJsonDocument::Compact));
      popupElements.append(pe);
    }
    QList<PopupExpression*> expressionInfos{};
    for (const QJsonValue& v : popupInfo["expressionInfos"].toArray())
    {
      QJsonObject o{v.toObject()};
      auto* pe = new PopupExpression(this);
      pe->setName(o["name"].toString());
      pe->setTitle(o["title"].toString());
      pe->setExpression(o["expression"].toString());

      const QString rt = o["returnType"].toString().toLower();
      pe->setReturnType(PopupExpressionReturnType::String);
      if (rt.compare(QStringLiteral("number")))
        pe->setReturnType(PopupExpressionReturnType::Number);
      else if (rt.compare(QStringLiteral("dictionary")))
        pe->setReturnType(PopupExpressionReturnType::Dictionary);

      expressionInfos.append(pe);
    }
    QList<PopupField*> fieldInfos{};
    for (const QJsonValue& v : popupInfo["fieldInfos"].toArray())
    {
      QJsonObject o{v.toObject()};
      auto* pf = new PopupField(this);
      pf->setFieldName(o["fieldName"].toString());
      pf->setEditable(o["isEditable"].toBool());
      pf->setLabel(o["label"].toString());
      pf->setVisible(o["visible"].isBool());
      if (o.contains("format"))
      {
        QJsonObject f{o["format"].toObject()};
        auto* pff = new PopupFieldFormat(this);
        pff->setUseThousandsSeparator(f["digitSeparator"].toBool());
        pff->setDecimalPlaces(f["places"].toInt());
        pf->setFormat(pff);
      }
      fieldInfos.append(pf);
    }
    auto* pd = new PopupDefinition(this);
    pd->setElements(popupElements);
    pd->setDescription(popupInfo["description"].toString());
    pd->setExpressions(expressionInfos);
    pd->setFields(fieldInfos);
    pd->setTitle(popupInfo["title"].toString());
    m_popupDefinitions[url] = pd;
  }

  return m_popupDefinitions[url];
}

Popup* IdentifyController::popup() const
{
  if (m_currentPopupIndex < 0 || m_currentPopupIndex >= static_cast<int>(m_popups.size()))
    return nullptr;

  return m_popups.at(m_currentPopupIndex);
}

} // Dsa
