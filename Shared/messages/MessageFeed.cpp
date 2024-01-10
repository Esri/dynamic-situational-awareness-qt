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

// C++ API Headers
#include "Error.h"
#include "Field.h"
#include "Domain.h"
#include "Renderer.h"
#include "SymbolTypes.h"
#include "ServiceTypes.h"
#include "SpatialReference.h"
#include "DynamicEntity.h"
#include "DynamicEntityInfo.h"
#include "DynamicEntityLayer.h"
#include "DynamicEntityDataSourceInfo.h"
#include "DynamicEntityObservation.h"
#include "DynamicEntityObservationInfo.h"
#include "AttributeListModel.h"

// DSA app headers
#include "Message.h"
#include "MessageFeed.h"
#include "MessagesOverlay.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::MessageFeed
  \inmodule Dsa
  \inherits DynamicEntityDataSource
  \brief Represents a feed for a given message type which will be displayed on a
  \l MessageOverlay.
 */

/*!
  \brief Constructor accepting a feed \a name, a message \a type, and \a overlay and an optional \a parent.
 */
MessageFeed::MessageFeed(const QString &name, const QString &type, QObject *parent) :
  DynamicEntityDataSource(parent),
  m_feedName(name),
  m_feedMessageType(type)
{
}

MessageFeed::~MessageFeed()
{
  // no cleanup currently required in destructor
}

QFuture<DynamicEntityDataSourceInfo*> MessageFeed::onLoadAsync()
{
  QList<QString> field_names;
  QString entity_id_field_name;
  m_isCoT = this->feedMessageType().compare(QStringLiteral("cot"), Qt::CaseInsensitive) == 0;
  if (m_isCoT)
  {
    // set the entity id field
    entity_id_field_name = Message::COT_UID_NAME;

    // set the fields
    field_names.reserve(8);
    field_names.emplace_back(Message::COT_TYPE_NAME);
    field_names.emplace_back(Message::COT_UID_NAME);
    field_names.emplace_back(Message::COT_POINT_NAME);
    field_names.emplace_back(Message::COT_POINT_LAT_NAME);
    field_names.emplace_back(Message::COT_POINT_LON_NAME);
    field_names.emplace_back(Message::COT_POINT_HAE_NAME);
  }
  else
  {
    // set the entity id field
    entity_id_field_name = Message::GEOMESSAGE_ID_NAME;

    // set the fields
    field_names.reserve(11);
    field_names.emplace_back(Message::GEOMESSAGE_TYPE_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_ACTION_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_UNIQUE_DESIGNATION_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_WKID_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_SIC_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_CONTROL_POINTS_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_UNIQUE_DESIGNATION_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_STATUS_911_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_ENVIRONMENT_NAME);
  }
  m_entityIdAttributeName = entity_id_field_name;
  field_names.emplace_back(entity_id_field_name);
  field_names.emplace_back(Message::SIDC_NAME);

  QList<Field> fields;
  fields.reserve(field_names.count());
  for (auto &fn : field_names)
  {
    fields.emplace_back(FieldType::Text, fn, "", 256, Domain(), false, true);
  }

  // build the dynamic entity data source info from the fields and the entity id field name
  auto dynamicEntityDataSourceInfo = new DynamicEntityDataSourceInfo(entity_id_field_name, fields, this);
  dynamicEntityDataSourceInfo->setSpatialReference(SpatialReference::wgs84());

  // listen for new entities
  connect(this, &DynamicEntityDataSource::dynamicEntityReceived, this, [this](DynamicEntityInfo *info)
  {
    // checck new entity for select/unselect action
    auto dynamicEntity = info->dynamicEntity();
    checkEntityForSelectAction(dynamicEntity);

    // mark the info as delete later so it can be cleaned up
    // info in the source cannot be cleaned up immediately since alert targets might need a reference to the related DynamicEntity
    info->deleteLater();
  });

  // listen for new observations
  connect(this, &DynamicEntityDataSource::dynamicEntityObservationReceived, this, [this](DynamicEntityObservationInfo *observationInfo)
  {
    // check new entity for select/unselect action
    auto dynamicEntity = observationInfo->observation()->dynamicEntity();
    checkEntityForSelectAction(dynamicEntity);

    // mark the observation as delete later so it can be cleaned up
    // observations in the source cannot be cleaned up immediately since alert targets might need a reference to the related DynamicEntity
    observationInfo->deleteLater();
  });

  // return the new source future
  return QtFuture::makeReadyFuture(dynamicEntityDataSourceInfo);
}

QFuture<void> MessageFeed::onConnectAsync()
{
  return QtFuture::makeReadyFuture();
}

QFuture<void> MessageFeed::onDisconnectAsync()
{
  return QtFuture::makeReadyFuture();
}

QString MessageFeed::feedName() const
{
  return m_feedName;
}

/*!
  \brief Sets the name of the feed to \a feedName.
 */
void MessageFeed::setFeedName(const QString& feedName)
{
  m_feedName = feedName;
}

/*!
  \brief Returns the type of this message feed.
 */
QString MessageFeed::feedMessageType() const
{
  return m_feedMessageType;
}

/*!
  \brief Sets the type of this message feed to \a feedMessageType.
 */
void MessageFeed::setFeedMessageType(const QString &feedMessageType)
{
  m_feedMessageType = feedMessageType;
}

/*!
  \brief Returns whether this message feed is visible.
 */
bool MessageFeed::isFeedVisible() const
{
  return m_messagesOverlay == nullptr ? false : m_messagesOverlay->isVisible();
}

/*!
  \brief Sets whether this message feed is visible to \a feedVisible.
 */
void MessageFeed::setFeedVisible(bool feedVisible)
{
  if (m_messagesOverlay == nullptr || m_messagesOverlay->isVisible() == feedVisible)
  {
    return;
  }
  m_messagesOverlay->setVisible(feedVisible);
}

/*!
  \brief Returns the \l MessagesOverlay for this feed.
 */
MessagesOverlay* MessageFeed::messagesOverlay() const
{
  return m_messagesOverlay;
}

/*!
  \brief Sets the \l MessagesOverlay for this feed to \a messagesOverlay.
 */
void MessageFeed::setMessagesOverlay(MessagesOverlay *messagesOverlay)
{
  m_messagesOverlay = messagesOverlay;
}

/*!
  \brief Returns the (local file) URL of the thumbnail to use for this feed.
 */
QUrl MessageFeed::thumbnailUrl() const
{
  return m_thumbnailUrl;
}

/*!
  \brief Sets the (local file) URL of the thumbnail to use for this feed to \a thumbnailUrl.
 */
void MessageFeed::setThumbnailUrl(const QUrl &thumbnailUrl)
{
  m_thumbnailUrl = thumbnailUrl;
}

/*!
  \brief Adds the \l Message \a message to the overlay. Returns whether adding was successful.
 */
bool MessageFeed::addMessage(const Message &message)
{
  static QString additionalErrorMessage = "DSA - MessageFeed";
  const auto messageId = message.messageId();
  if (messageId.isEmpty())
  {
    emit errorOccurred(Error("Failed to add message - message ID is empty", additionalErrorMessage, ExtendedErrorType::None));
    return false;
  }

  if (message.messageType() != this->feedMessageType())
  {
    emit errorOccurred(Error("Failed to add message - message type mismatch", additionalErrorMessage, ExtendedErrorType::None));
    return false;
  }

  const auto symbolId = message.symbolId();
  const auto geometry = message.geometry();
  const auto messageAction = message.messageAction();

  switch (messageAction)
  {
  case Message::MessageAction::Remove:
    this->deleteEntityAsync(messageId).then([]{});
    return true;

  default:
    if (m_messagesOverlay->renderer() && m_messagesOverlay->renderer()->rendererType() == RendererType::DictionaryRenderer && symbolId.isEmpty())
    {
      emit errorOccurred(Error("Failed to add message - symbol ID is empty", additionalErrorMessage, ExtendedErrorType::None));
      return false;
    }

    if (geometry.isEmpty())
    {
      emit errorOccurred(Error("Failed to add message - geometry is empty", additionalErrorMessage, ExtendedErrorType::None));
      return false;
    }

    if (geometry.geometryType() != GeometryType::Point)
    {
      emit errorOccurred(Error("Failed to add message - only point geometry types are supported", additionalErrorMessage, ExtendedErrorType::None));
      return false;
    }

    addObservation(geometry, message.attributes());
  }

  return true;
}

void MessageFeed::checkEntityForSelectAction(Esri::ArcGISRuntime::DynamicEntity *dynamicEntity)
{
  // check for selection on add for geomessage types only
  if (!m_isCoT)
  {
    // find the action attribute
    if (dynamicEntity)
    {
      auto actionValue = dynamicEntity->attributes()->attributesMap()[Message::GEOMESSAGE_ACTION_NAME].toString();
      static QString selectValue = Message::fromMessageAction(Message::MessageAction::Select);
      static QString unselectValue = Message::fromMessageAction(Message::MessageAction::Unselect);
      if (actionValue.compare(selectValue, Qt::CaseInsensitive) == 0)
      {
        m_messagesOverlay->dynamicEntityLayer()->selectDynamicEntity(dynamicEntity);
      }
      else if (actionValue.compare(unselectValue, Qt::CaseInsensitive) == 0)
      {
        m_messagesOverlay->dynamicEntityLayer()->unselectDynamicEntity(dynamicEntity);
      }
    }
  }
}

} // Dsa
