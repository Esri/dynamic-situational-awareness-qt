// Copyright 2018 ESRI
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

// PCH header
#include "pch.hpp"

#include "OptionsController.h"

// example app headers
#include "AppConstants.h"
#include "LocationTextController.h"
#include "MessageFeed.h"
#include "MessageFeedListModel.h"
#include "MessageFeedsController.h"
#include "MessagesOverlay.h"

// toolkit headers
#include "CoordinateConversionConstants.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// C++ API headers
#include "DictionaryRenderer.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class OptionsController
  \inherits Toolkit::AbstractTool
  \brief Tool controller for setting various options for the app.
 */

/*!
 \brief Constructor that takes an optional \a parent.
 */
OptionsController::OptionsController(QObject* parent) :
  Toolkit::AbstractTool(parent),
  m_coordinateFormatOptions{Toolkit::CoordinateConversionConstants::DEGREES_MINUTES_SECONDS_FORMAT,
                            Toolkit::CoordinateConversionConstants::DECIMAL_DEGREES_FORMAT,
                            Toolkit::CoordinateConversionConstants::DEGREES_DECIMAL_MINUTES_FORMAT,
                            Toolkit::CoordinateConversionConstants::UTM_FORMAT,
                            Toolkit::CoordinateConversionConstants::MGRS_FORMAT,
                            Toolkit::CoordinateConversionConstants::USNG_FORMAT,
                            Toolkit::CoordinateConversionConstants::GEOREF_FORMAT,
                            Toolkit::CoordinateConversionConstants::GARS_FORMAT},
  m_units{AppConstants::UNIT_METERS,
          AppConstants::UNIT_FEET}
{
  Toolkit::ToolManager::instance().addTool(this);
  emit unitsChanged();
  emit coordinateFormatsChanged();
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
  m_locationTextController = Toolkit::ToolManager::instance().tool<LocationTextController>();
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
  m_coordinateFormat = properties[Toolkit::CoordinateConversionConstants::COORDINATE_FORMAT_PROPERTY].toString();
  if (m_coordinateFormat.isEmpty())
    m_coordinateFormat = Toolkit::CoordinateConversionConstants::DEGREES_MINUTES_SECONDS_FORMAT;

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
 \brief Returns the initial index.
  This is used to set the initial index in the combo box to match the controller.
  */
int OptionsController::initialFormatIndex() const
{
  return m_initialFormatIndex;
}

/*!
 \brief Returns the initial index of the list of units.

  This is used to set the initial index in the combo box to match the controller.
  */
int OptionsController::initialUnitIndex() const
{
  return m_initialUnitIndex;
}

/*!
 \brief Returns whether the friendly tracks labels show.
*/
bool OptionsController::showFriendlyTracksLabels()
{
  auto renderers = friendlyTracksOverlayRenderers();
  if (renderers.isEmpty())
    return true;

  // just check the first renderer in the list to determine label visibility
  return renderers[0]->isTextVisible();
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
    renderer->setTextVisible(show);
}

/*!
 \brief Returns the DictionaryRenderer from the friendly tracks MessageFeed.
*/
QList<DictionaryRenderer*> OptionsController::friendlyTracksOverlayRenderers() const
{
  QList<DictionaryRenderer*> renderers;

  // Obtain and cache the MessageFeedsController. Connect members and emit signals so properties update
  MessageFeedsController* messageController = Toolkit::ToolManager::instance().tool<MessageFeedsController>();
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
