
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

#include "OptionsController.h"

// dsa app headers
#include "AppConstants.h"
#include "LocationTextController.h"
#include "MessageFeed.h"
#include "MessageFeedListModel.h"
#include "MessageFeedsController.h"
#include "MessagesOverlay.h"
#include "ToolManager.h"

// toolkit headers
#include "CoordinateConversionConstants.h"

// C++ API headers
#include "DictionaryRenderer.h"
#include "DictionarySymbolStyle.h"
#include "DictionarySymbolStyleConfiguration.h"
#include "SymbolTypes.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {


namespace
{
  // Returns whether the render is showing text labels for military symbols.
  bool isTextVisible(DictionaryRenderer* renderer)
  {
    auto configurations = renderer->dictionarySymbolStyle()->configurations();
    auto it = std::find_if(configurations.cbegin(), configurations.cend(),
                           [](auto c) { return c->name() == QString("text"); } );

    if (it == configurations.end())
      return false;

    if ((*it)->value().toString() == QString("ON"))
      return true;
    else
      return false;
  }


  // Sets whether the render is showing text labels for military symbols.
  void setTextVisible(DictionaryRenderer* renderer, bool visible)
  {
    auto configurations = renderer->dictionarySymbolStyle()->configurations();
    auto it = std::find_if(configurations.cbegin(), configurations.cend(),
                           [](auto c) { return c->name() == QString("text"); } );

    if (it == configurations.end())
      return;

    (*it)->setValue(visible ? QString("ON")
                            : QString("OFF"));
  }
}


/*!
  \class Dsa::OptionsController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for setting various options for the app.
 */

/*!
 \brief Constructor that takes an optional \a parent.
 */
OptionsController::OptionsController(QObject* parent) :
  AbstractTool(parent),
  m_coordinateFormatOptions{Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::DEGREES_MINUTES_SECONDS_FORMAT,
                            Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::DECIMAL_DEGREES_FORMAT,
                            Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::DEGREES_DECIMAL_MINUTES_FORMAT,
                            Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::UTM_FORMAT,
                            Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::MGRS_FORMAT,
                            Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::USNG_FORMAT,
                            Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::GEOREF_FORMAT,
                            Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::GARS_FORMAT},
  m_units{AppConstants::UNIT_METERS,
          AppConstants::UNIT_FEET}
{
  emit unitsChanged();
  emit coordinateFormatsChanged();
  ToolManager::instance().addTool(this);
}

/*!
 \brief Destructor
 */
OptionsController::~OptionsController()
{
}

/*!
 \brief Obtains the update tool from the tool manager and sets up connections to the various signals.
 */
void OptionsController::getUpdatedTools()
{
  // Obtain and cache the LocationTextController. Connect members and emit signals so properties update
  m_locationTextController = ToolManager::instance().tool<LocationTextController>();
  if (m_locationTextController)
  {
    connect(m_locationTextController, &LocationTextController::useGpsForElevationChanged, this, [this]
    {
      emit useGpsForElevationChanged();
    });

    // set initial format
    setCoordinateFormat(m_coordinateFormat);
    setUnitOfMeasurement(m_unitOfMeasurement);

    emit useGpsForElevationChanged();
  }
}

/*!
 \brief Returns the tool name - \c "Options Tool".
 */
QString OptionsController::toolName() const
{
  return "Options Tool";
}

/*!
 \brief Sets \a properties from the configuration file.
 */
void OptionsController::setProperties(const QVariantMap& properties)
{
  // access tool properties from the config
  m_coordinateFormat = properties["CoordinateFormat"].toString();
  if (m_coordinateFormat.isEmpty())
    m_coordinateFormat = Esri::ArcGISRuntime::Toolkit::CoordinateConversionConstants::DEGREES_MINUTES_SECONDS_FORMAT;

  m_unitOfMeasurement = properties[AppConstants::UNIT_OF_MEASUREMENT_PROPERTYNAME].toString();
  if (m_unitOfMeasurement.isEmpty())
    m_unitOfMeasurement = AppConstants::UNIT_METERS;

  // update properties
  m_initialUnitIndex = m_units.indexOf(m_unitOfMeasurement);
  emit initialUnitIndexChanged();
  m_initialFormatIndex = m_coordinateFormatOptions.indexOf(m_coordinateFormat);
  emit initialFormatIndex();

  auto userNameFindIt = properties.find(AppConstants::USERNAME_PROPERTYNAME);
  if (userNameFindIt != properties.end())
    setUserName(userNameFindIt.value().toString());

  // get access to the various tool controllers
  getUpdatedTools();
}

/*!
 \property OptionsController::coordinateFormats
 \brief Returns the coordinate format list for display in the combo box.
 */
QStringList OptionsController::coordinateFormats() const
{
  return m_coordinateFormatOptions;
}

/*!
 \brief Sets the current coordinate \a format to be used.
 */
void OptionsController::setCoordinateFormat(const QString& format)
{
  if (!m_locationTextController)
    return;

  m_locationTextController->setCoordinateFormat(format);
}

/*!
  \property OptionsController::useGpsForElevation
 \brief Returns whether to use GPS for elevation for display.
 */
bool OptionsController::useGpsForElevation() const
{
  if (!m_locationTextController)
    return false;

  return m_locationTextController->useGpsForElevation();
}

/*!
 \brief Sets whether to \a useGps for elevation display.
 */
void OptionsController::setUseGpsForElevation(bool useGps)
{
  if (!m_locationTextController)
    return;

  m_locationTextController->setUseGpsForElevation(useGps);
}

/*!
  \property OptionsController::units
 \brief Returns the unit of measurement list for display.
 */
QStringList OptionsController::units() const
{
  return m_units;
}

/*!
 \brief Sets the \a unit of measurement.
 */
void OptionsController::setUnitOfMeasurement(const QString& unit)
{
  if (!m_locationTextController)
    return;

  m_locationTextController->setUnitOfMeasurement(unit);
}

/*!
 \property OptionsController::userName
 \brief Returns the user name for the app.
 */
QString OptionsController::userName() const
{
  return m_userName;
}

/*!
 \brief Sets the \a userName for the app.
 */
void OptionsController::setUserName(const QString& userName)
{
  if (userName == m_userName)
    return;

  m_userName = userName;
  emit userNameChanged();
  emit propertyChanged(AppConstants::USERNAME_PROPERTYNAME, m_userName);
}

/*!
  \property OptionsController::initialFormatIndex
 \brief Returns the initial index.
  This is used to set the initial index in the combo box to match the controller.
  */
int OptionsController::initialFormatIndex() const
{
  return m_initialFormatIndex;
}

/*!
  \property OptionsController::initialUnitIndex
 \brief Returns the initial index of the list of units.

  This is used to set the initial index in the combo box to match the controller.
  */
int OptionsController::initialUnitIndex() const
{
  return m_initialUnitIndex;
}

/*!
  \property OptionsController::showFriendlyTracksLabels
 \brief Returns whether the friendly tracks labels show.
*/
bool OptionsController::showFriendlyTracksLabels()
{
  auto renderers = friendlyTracksOverlayRenderers();
  if (renderers.isEmpty())
    return true;

  // just check the first renderer in the list to determine label visibility
  return Dsa::isTextVisible(renderers[0]);
}

/*!
 \brief Sets whether the friendly tracks labels \a show.
*/
void OptionsController::setShowFriendlyTracksLabels(bool show)
{
  auto renderers = friendlyTracksOverlayRenderers();
  if (renderers.isEmpty())
    return;

  for (auto renderer : renderers)
    Dsa::setTextVisible(renderer, show);
}

/*!
 \brief Returns the DictionaryRenderer from the friendly tracks MessageFeed.
*/
QList<DictionaryRenderer*> OptionsController::friendlyTracksOverlayRenderers() const
{
  QList<DictionaryRenderer*> renderers;

  // Obtain and cache the MessageFeedsController. Connect members and emit signals so properties update
  MessageFeedsController* messageController = ToolManager::instance().tool<MessageFeedsController>();
  if (messageController)
  {
    MessageFeedListModel* messageFeedModel = dynamic_cast<MessageFeedListModel*>(messageController->messageFeeds());
    if (messageFeedModel)
    {
      for (int i = 0; i < messageFeedModel->count(); i++)
      {
        MessageFeed* feed = messageFeedModel->at(i);
        if (QString(feed->feedName()).contains("friendly tracks", Qt::CaseInsensitive))
        {
          Renderer* renderer = feed->messagesOverlay()->renderer();
          if (renderer && renderer->rendererType() == RendererType::DictionaryRenderer)
            renderers.append(dynamic_cast<DictionaryRenderer*>(renderer));
        }
      }
    }
  }

  return renderers;
}

} // Dsa

// Signal Documentation
/*!
  \fn void OptionsController::coordinateFormatsChanged();
  \brief Signal emitted when the coordinateFormats property changes.
 */

/*!
  \fn void OptionsController::useGpsForElevationChanged();
  \brief Signal emitted when the useGpsForElevation property changes.
 */

/*!
  \fn void OptionsController::unitsChanged();
  \brief Signal emitted when the units property changes.
 */

/*!
  \fn void OptionsController::initialUnitIndexChanged();
  \brief Signal emitted when the initial unit index changes.
 */

/*!
  \fn void OptionsController::initialFormatIndexChanged();
  \brief Signal emitted when the initial coordinate format index changes.
 */

/*!
  \fn void OptionsController::showFriendlyTracksLabelsChanged();
  \brief Signal emitted when showFriendlyTracksLabels property changes.
 */

/*!
  \fn void OptionsController::userNameChanged();
  \brief Signal emitted when the userName property changes.
 */
