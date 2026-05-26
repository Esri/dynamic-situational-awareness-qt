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

#include "MessageFeed.h"

// C++ API
#include "AttributeListModel.h"
#include "Domain.h"
#include "DictionaryRenderer.h"
#include "DictionarySymbolStyle.h"
#include "DynamicEntity.h"
#include "DynamicEntityDataSourceInfo.h"
#include "DynamicEntityInfo.h"
#include "DynamicEntityObservation.h"
#include "DynamicEntityObservationInfo.h"
#include "Error.h"
#include "Field.h"
#include "LayerSceneProperties.h"
#include "PictureMarkerSymbol.h"
#include "Renderer.h"
#include "SceneViewTypes.h"
#include "ServiceTypes.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "SpatialReference.h"
#include "SymbolTypes.h"
#include "TrackDisplayProperties.h"
// DSA
#include "Message.h"
#include "MessageFeedConstants.h"
#include "MessagesOverlay.h"
// Qt
#include <QFile>
#include <QFileInfo>
// STD
#include <algorithm>
#include <iterator>
#include <memory>
#include <tuple>
#include <vector>

using namespace Esri::ArcGISRuntime;

namespace Dsa {

MessageFeed::MessageFeed(const QVariantMap& properties, const QString& resourcePath, QObject* parent):
  DynamicEntityDataSource(parent), m_resourcePath(resourcePath)
{
  using namespace MessageFeedConstants;
  qint8 requiredProperties = 0;
  qint8 requiredPropertiesValid = 0;

  // set all the string properties and keep track of any required that are not found
  // tuple<JsonName, Member*, Required, Default, AllowedValues>
  using PropString = std::tuple<const QString&, QString&, bool, const QString&, const QStringList&>;
  const QStringList empty{};
  const std::vector<PropString> stringProperties{
    { MESSAGE_FEEDS_NAME, m_feedName, true, QString{}, empty },
    { MESSAGE_FEEDS_TYPE, m_feedMessageType, true, QString{}, empty },
    { MESSAGE_FEEDS_RENDERER, m_renderer, true, QString{}, empty },
    { MESSAGE_FEEDS_THUMBNAIL, m_thumbnail, false, QString{}, empty },
    { MESSAGE_FEEDS_PLACEMENT, m_surfacePlacement, false, MESSAGE_FEEDS_PLACEMENT_DEFAULT, empty },
    { MESSAGE_FEEDS_OBSERVATIONS_COLOR, m_colorObservations, false, MESSAGE_FEEDS_TRACK_DISPLAY_COLOR_DEFAULT, MESSAGE_FEEDS_TRACK_DISPLAY_COLORS },
    { MESSAGE_FEEDS_TRACK_LINE_COLOR, m_colorTrackLine, false, MESSAGE_FEEDS_TRACK_DISPLAY_COLOR_DEFAULT, MESSAGE_FEEDS_TRACK_DISPLAY_COLORS },
  };
  for (const PropString& ps : stringProperties)
  {
    QString& pMember = std::get<QString&>(ps);
    pMember = std::get<3>(ps);
    const QString& propertyName = std::get<0>(ps);
    const bool propertyRequired = std::get<bool>(ps);
    requiredProperties += propertyRequired ? 1 : 0;
    if (const QString s = properties[propertyName].toString(); !s.isEmpty())
    {
      const auto v = std::get<const QStringList&>(ps);
      if (!v.empty() && !v.contains(s))
        return;

      pMember = s;
      if (propertyRequired)
        ++requiredPropertiesValid;
    }
  }

  // all message feed configurations are required to have name, type and renderer
  m_configurationWasValid = requiredPropertiesValid == requiredProperties;
  if (!m_configurationWasValid)
    return;

  // build the thumbnail url if it was not empty
  if (!m_thumbnail.isEmpty())
  {
    if (QFile::exists(QString{QStringLiteral(":/Resources/icons/xhdpi/message/%1")}.arg(m_thumbnail)))
      setThumbnailUrl(QString{QStringLiteral("qrc:/Resources/icons/xhdpi/message/%1")}.arg(m_thumbnail));
    else if (QFile::exists(QString{QStringLiteral("%1/icons/%2")}.arg(m_resourcePath, m_thumbnail)))
      setThumbnailUrl(QUrl::fromLocalFile(QString{QStringLiteral("%1/icons/%2")}.arg(m_resourcePath, m_thumbnail)));
  }

  m_messagesOverlay = new MessagesOverlay(this, m_feedMessageType, this);

  // tuple<JsonName, Member*, Default>
  using PropBool = std::tuple<QString, bool&, bool>;
  const std::vector<PropBool> boolProperties{
    { MESSAGE_FEEDS_OBSERVATIONS_SHOW, m_showPreviousObservations, false },
    { MESSAGE_FEEDS_TRACK_LINE_SHOW, m_showTrackLine, false },
  };
  for (const PropBool& pb : boolProperties)
  {
    bool& pMember = std::get<bool&>(pb);
    pMember = std::get<bool>(pb);
    if (const QVariant v = properties[std::get<QString>(pb)]; v.canConvert<bool>())
      pMember = v.toBool();
  }

  // tuple<JsonName, Member*, Default>
  using PropInt = std::tuple<QString, int&, int>;
  const std::vector<PropInt> intProperties{
    { MESSAGE_FEEDS_OBSERVATIONS_SIZE, m_sizeObservations, 10 },
    { MESSAGE_FEEDS_OBSERVATIONS_MAXIMUM, m_maximumObservations, 5 },
    { MESSAGE_FEEDS_TRACK_LINE_SIZE, m_sizeTrackLine, 4 },
  };
  for (const PropInt& pi : intProperties)
  {
    int& pMember = std::get<int&>(pi);
    pMember = std::get<int>(pi);
    if (const QVariant v = properties[std::get<QString>(pi)]; v.canConvert<int>())
      pMember = v.toInt();
  }

  // turn the layer on and create/assign it's renderer
  setFeedVisible(m_isFeedVisible);
  m_messagesOverlay->setRenderer(createRenderer());

  // update the surface placement and set the overlay scene properties
  SurfacePlacement surfacePlacement = SurfacePlacement::DrapedBillboarded;
  if (m_surfacePlacement.compare("relative", Qt::CaseInsensitive) == 0)
    surfacePlacement = SurfacePlacement::Relative;
  else if (m_surfacePlacement.compare("absolute", Qt::CaseInsensitive) == 0)
    surfacePlacement = SurfacePlacement::Absolute;
  m_messagesOverlay->setSceneProperties(LayerSceneProperties(surfacePlacement));

  // initialize the track display renderer properties
  // OBSERVATIONS
  TrackDisplayProperties* tdp = m_messagesOverlay->trackDisplayProperties();
  tdp->setShowPreviousObservations(m_showPreviousObservations);
  updateSymbolObservations();
  // TRACKLINE
  tdp->setShowTrackLine(m_showTrackLine);
  updateSymbolTrackLine();
  // LIMITS
  int maximumObservations = m_maximumObservations == 0 ? std::numeric_limits<int>::max() : m_maximumObservations;
  tdp->setMaximumObservations(maximumObservations);
}

MessageFeed::~MessageFeed() = default;

QFuture<DynamicEntityDataSourceInfo*> MessageFeed::onLoadAsync()
{
  std::vector<QString> field_names{};
  if (m_isCoT = feedMessageType().compare(QStringLiteral("cot"), Qt::CaseInsensitive) == 0; m_isCoT)
  {
    // set the entity id field
    m_entityIdAttributeName = Message::COT_UID_NAME;
    m_searchAttributeName = Message::COT_UID_NAME;

    // set the fields
    field_names.reserve(7);
    field_names.emplace_back(Message::COT_TYPE_NAME);
    field_names.emplace_back(Message::COT_POINT_NAME);
    field_names.emplace_back(Message::COT_POINT_LAT_NAME);
    field_names.emplace_back(Message::COT_POINT_LON_NAME);
    field_names.emplace_back(Message::COT_POINT_HAE_NAME);
  }
  else
  {
    // set the entity id field
    m_entityIdAttributeName = Message::GEOMESSAGE_ID_NAME;
    m_searchAttributeName = Message::GEOMESSAGE_UNIQUE_DESIGNATION_NAME;

    // set the fields
    field_names.reserve(10);
    field_names.emplace_back(Message::GEOMESSAGE_TYPE_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_ACTION_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_UNIQUE_DESIGNATION_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_WKID_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_SIC_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_CONTROL_POINTS_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_STATUS_911_NAME);
    field_names.emplace_back(Message::GEOMESSAGE_ENVIRONMENT_NAME);
  }
  field_names.emplace_back(m_entityIdAttributeName);
  field_names.emplace_back(Message::SIDC_NAME);

  QList<Field> fields;
  fields.reserve(field_names.size());
  for (const QString& fn : field_names)
  {
    fields.emplace_back(FieldType::Text, fn, fn, 256, Domain{}, false, true);
  }

  // build the dynamic entity data source info from the fields and the entity id field name
  auto* dynamicEntityDataSourceInfo = new DynamicEntityDataSourceInfo(m_entityIdAttributeName, fields, this);
  dynamicEntityDataSourceInfo->setSpatialReference(SpatialReference::wgs84());

  // listen for new entities
  connect(this, &DynamicEntityDataSource::dynamicEntityReceived, this, [this](DynamicEntityInfo* info)
  {
    // check new entity for select/unselect action
    auto* dynamicEntity = info->dynamicEntity();
    dynamicEntity->setParent(this);
    m_dynamicEntities[dynamicEntity->entityId()] = dynamicEntity;
    checkEntityForSelectAction(dynamicEntity);

    // mark the info as delete later so it can be cleaned up
    // info in the source cannot be cleaned up immediately since alert targets might need a reference to the related DynamicEntity
    info->deleteLater();
  });

  // listen for new observations
  connect(this, &DynamicEntityDataSource::dynamicEntityObservationReceived, this, [this](DynamicEntityObservationInfo* observationInfo)
  {
    // check new entity for select/unselect action
    auto* dynamicEntity = observationInfo->observation()->dynamicEntity();
    checkEntityForSelectAction(dynamicEntity);

    // mark the observation as delete later so it can be cleaned up
    // observations in the source cannot be cleaned up immediately since alert targets might need a reference to the related DynamicEntity
    observationInfo->deleteLater();
  });

  // remove ownership when entities are purged
  connect(this, &DynamicEntityDataSource::dynamicEntityPurged, this, [this](DynamicEntityInfo* info)
  {
    // release the dynamic entity
    auto* dynamicEntity = info->dynamicEntity();
    m_dynamicEntities.remove(dynamicEntity->entityId());
    dynamicEntity->deleteLater();

    // mark the info as delete later so it can be cleaned up
    info->deleteLater();
  });

  // return the new source future
  return QtFuture::makeReadyValueFuture(dynamicEntityDataSourceInfo);
}

QFuture<void> MessageFeed::onConnectAsync()
{
  return QtFuture::makeReadyVoidFuture();
}

QFuture<void> MessageFeed::onDisconnectAsync()
{
  return QtFuture::makeReadyVoidFuture();
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
void MessageFeed::setFeedMessageType(const QString& feedMessageType)
{
  m_feedMessageType = feedMessageType;
}

/*!
  \brief Returns whether this message feed is visible.
 */
bool MessageFeed::isFeedVisible() const
{
  return m_isFeedVisible;
}

/*!
  \brief Sets whether this message feed is visible to \a feedVisible.
 */
void MessageFeed::setFeedVisible(bool feedVisible)
{
  if (m_isFeedVisible == feedVisible)
    return;

  m_isFeedVisible = feedVisible;
  emit feedChanged();

  if (m_messagesOverlay == nullptr)
    return;
  if (m_messagesOverlay->isVisible() == feedVisible)
    return;

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
void MessageFeed::setMessagesOverlay(MessagesOverlay* messagesOverlay)
{
  m_messagesOverlay = messagesOverlay;
}

QString MessageFeed::thumbnail() const
{
  return m_thumbnail;
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
void MessageFeed::setThumbnailUrl(const QUrl& thumbnailUrl)
{
  m_thumbnailUrl = thumbnailUrl;
}

QString MessageFeed::renderer() const
{
  return m_renderer;
}

QString MessageFeed::surfacePlacement() const
{
  return m_surfacePlacement;
}

Renderer* MessageFeed::createRenderer()
{
  // hold mil2525 symbol styles as statics to be shared by multiple renderers if needed
  QObject* parent = dynamic_cast<QObject*>(this);

  if (m_renderer.compare("mil2525c", Qt::CaseInsensitive) == 0)
  {
    if (!MessageFeed::s_dictionarySymbolStyleMil2525c)
    {
      const QString stylePath = QString{QStringLiteral("%1/styles/arcade/mil2525c.stylx")}.arg(m_resourcePath);
      if (!QFileInfo::exists(stylePath))
      {
        emit errorOccurred(Error(QStringLiteral("mil2525c.stylx not found"), QString{QStringLiteral("Could not find %1")}.arg(stylePath), ExtendedErrorType::None));
        return nullptr;
      }

      s_dictionarySymbolStyleMil2525c = DictionarySymbolStyle::createFromFile(stylePath, parent);
    }

    return new DictionaryRenderer(s_dictionarySymbolStyleMil2525c, parent);
  }
  else if (m_renderer.compare("mil2525d", Qt::CaseInsensitive) == 0)
  {
    if (!s_dictionarySymbolStyleMil2525d)
    {
      const QString stylePath = QString{QStringLiteral("%1/styles/arcade/mil2525d.stylx")}.arg(m_resourcePath);
      if (!QFileInfo::exists(stylePath))
      {
        emit errorOccurred(Error(QStringLiteral("mil2525d.stylx not found"), QString{QStringLiteral("Could not find %1")}.arg(stylePath), ExtendedErrorType::None));
        return nullptr;
      }

      s_dictionarySymbolStyleMil2525d = DictionarySymbolStyle::createFromFile(stylePath, parent);
    }

    return new DictionaryRenderer(s_dictionarySymbolStyleMil2525d, parent);
  }

  // else default to simple renderer with picture marker symbol
  PictureMarkerSymbol* symbol = nullptr;
  const QString qrcFile = QString{QStringLiteral(":/Resources/icons/xhdpi/message/%1")}.arg(m_renderer);

  if (QFile::exists(qrcFile))
  {
    symbol = new PictureMarkerSymbol(QImage(qrcFile), parent);
  }
  else
  {
    const QString dataFile = m_resourcePath + QString{QStringLiteral("/icons/%1")}.arg(m_renderer);
    if (QFile::exists(dataFile))
      symbol = new PictureMarkerSymbol(QImage(dataFile), parent);
  }

  if (symbol == nullptr)
    return nullptr;

  symbol->setWidth(40.0f);
  symbol->setHeight(40.0f);
  return new SimpleRenderer(symbol, parent);
}

bool MessageFeed::addMessage(const Message& message)
{
  static QString additionalErrorMessage = "DSA - MessageFeed";
  const auto messageId = message.messageId();
  if (messageId.isEmpty())
  {
    emit errorOccurred(Error("Failed to add message - message ID is empty", additionalErrorMessage, ExtendedErrorType::None));
    return false;
  }

  if (message.messageType() != feedMessageType())
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
    {
      const auto future = deleteEntityAsync(messageId);
      Q_UNUSED(future);
    }
    return true;

  default:
    if (m_messagesOverlay == nullptr)
    {
      emit errorOccurred(Error("MessagesOverlay not set", additionalErrorMessage, ExtendedErrorType::None));
      return false;
    }

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

bool MessageFeed::showPreviousObservations() const
{
  return m_showPreviousObservations;
}

void MessageFeed::setShowPreviousObservations(bool showPreviousObservations)
{
  if (m_showPreviousObservations == showPreviousObservations)
    return;

  m_showPreviousObservations = showPreviousObservations;

  if (!m_messagesOverlay)
    return;

  m_messagesOverlay->trackDisplayProperties()->setShowPreviousObservations(showPreviousObservations);
  emit feedChanged();
}

int MessageFeed::maximumObservations() const
{
  int maximumObservations = m_maximumObservations;
  if (maximumObservations == std::numeric_limits<int>::max())
    maximumObservations = 0;

  return maximumObservations;
}

void MessageFeed::setMaximumObservations(int maximumObservations)
{
  if (m_maximumObservations == maximumObservations)
    return;

  if (maximumObservations == 0)
    maximumObservations = std::numeric_limits<int>::max();

  m_maximumObservations = maximumObservations;

  if (!m_messagesOverlay)
    return;

  m_messagesOverlay->trackDisplayProperties()->setMaximumObservations(maximumObservations);
  emit feedChanged();
}

bool MessageFeed::showTrackLine() const
{
  return m_showTrackLine;
}

void MessageFeed::setShowTrackLine(bool showTrackLine)
{
  if (m_showTrackLine == showTrackLine)
    return;

  m_showTrackLine = showTrackLine;

  if (!m_messagesOverlay)
    return;

  m_messagesOverlay->trackDisplayProperties()->setShowTrackLine(showTrackLine);
  emit feedChanged();
}

QString MessageFeed::colorObservations() const
{
  return m_colorObservations;
}

void MessageFeed::setColorObservations(const QString& color)
{
  if (m_colorObservations == color)
    return;

  m_colorObservations = color;
  updateSymbolObservations();
}

int MessageFeed::sizeObservations() const
{
  return m_sizeObservations;
}

void MessageFeed::setSizeObservations(int symbolSize)
{
  if (m_sizeObservations == symbolSize)
    return;

  m_sizeObservations = symbolSize;
  updateSymbolObservations();
}

void MessageFeed::updateSymbolObservations()
{
  if (!m_messagesOverlay)
    return;

  QColor c{m_colorObservations};
  if (!c.isValid())
    return;

  SimpleRenderer* renderer = dynamic_cast<SimpleRenderer*>(m_messagesOverlay->trackDisplayProperties()->previousObservationRenderer());
  auto* symbol = new SimpleMarkerSymbol(this);
  symbol->setColor(c);
  symbol->setSize(static_cast<float>(m_sizeObservations));
  auto* previousSymbol = renderer->symbol();
  renderer->setSymbol(symbol);

  if (previousSymbol)
    previousSymbol->deleteLater();

  emit feedChanged();
}

QString MessageFeed::colorTrackLine() const
{
  return m_colorTrackLine;
}

void MessageFeed::setColorTrackLine(const QString& color)
{
  if (m_colorTrackLine == color)
    return;

  m_colorTrackLine = color;
  updateSymbolTrackLine();
}

int MessageFeed::sizeTrackLine() const
{
  return m_sizeTrackLine;
}

void MessageFeed::setSizeTrackLine(int symbolSize)
{
  if (m_sizeTrackLine == symbolSize)
    return;

  m_sizeTrackLine = symbolSize;
  updateSymbolTrackLine();
}

void MessageFeed::updateSymbolTrackLine()
{
  if (!m_messagesOverlay)
    return;

  QColor c{m_colorTrackLine};
  if (!c.isValid())
    return;

  SimpleRenderer* renderer = dynamic_cast<SimpleRenderer*>(m_messagesOverlay->trackDisplayProperties()->trackLineRenderer());
  auto* symbol = new SimpleLineSymbol(this);
  symbol->setColor(c);
  symbol->setWidth(static_cast<float>(m_sizeTrackLine));
  auto* previousSymbol = renderer->symbol();
  renderer->setSymbol(symbol);

  if (previousSymbol)
    previousSymbol->deleteLater();

  emit feedChanged();
}

QString MessageFeed::searchAttributeName() const
{
  return m_searchAttributeName;
}

bool MessageFeed::configurationWasValid() const
{
  return m_configurationWasValid;
}

QString MessageFeed::entityIdAttributeName() const
{
  return m_entityIdAttributeName;
}

/*!
 * \brief Gets a pointer to a DynamicEntity by it's entity ID that was defined in the feed setup. Used for selection in alerts, etc.
 * \param entityId
 * \return
 */
DynamicEntity* MessageFeed::getDynamicEntityById(quint64 entityId) const
{
  return m_dynamicEntities.contains(entityId) ? m_dynamicEntities[entityId] : nullptr;
}

/*!
 * \brief Returns a reference to the list of active entities. Used when a new alert is setup so any already existing entities can be searched and evaluated
 * \return
 */
const QHash<quint64, DynamicEntity*>& MessageFeed::dynamicEntities() const
{
  return m_dynamicEntities;
}

/*!
 * \brief Checks a cursor on target message for the 'Select' action type and selects it in the overlay(DynamicEntityLayer)
 * \param dynamicEntity
 */
void MessageFeed::checkEntityForSelectAction(DynamicEntity* dynamicEntity)
{
  // check for selection on add for geomessage types only
  if (!m_isCoT)
    return;

  // find the action attribute
  if (dynamicEntity)
  {
    const auto actionValue = dynamicEntity->attributes()->attributesMap()[Message::GEOMESSAGE_ACTION_NAME].toString();
    static const QString selectValue = Message::fromMessageAction(Message::MessageAction::Select);
    static const QString unselectValue = Message::fromMessageAction(Message::MessageAction::Unselect);
    if (actionValue.compare(selectValue, Qt::CaseInsensitive) == 0)
      m_messagesOverlay->selectDynamicEntity(dynamicEntity);
    else if (actionValue.compare(unselectValue, Qt::CaseInsensitive) == 0)
      m_messagesOverlay->unselectDynamicEntity(dynamicEntity);
  }
}

} // Dsa
