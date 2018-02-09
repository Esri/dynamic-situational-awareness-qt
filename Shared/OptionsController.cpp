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

#include "OptionsController.h"
#include "ToolResourceProvider.h"
#include "ToolManager.h"
#include "LocationTextController.h"
#include "MessageFeedsController.h"
#include "MessagesOverlay.h"
#include "MessageFeed.h"
#include "MessageFeedListModel.h"
#include "DictionaryRenderer.h"

using namespace Esri::ArcGISRuntime;

const QString OptionsController::COORDINATE_FORMAT_PROPERTYNAME = QStringLiteral("CoordinateFormat");
const QString OptionsController::UNIT_OF_MEASUREMENT_PROPERTYNAME = QStringLiteral("UnitOfMeasurement");
const QString OptionsController::DMS = QStringLiteral("DMS");
const QString OptionsController::DD = QStringLiteral("DD");
const QString OptionsController::DDM = QStringLiteral("DDM");
const QString OptionsController::UTM = QStringLiteral("UTM");
const QString OptionsController::MGRS = QStringLiteral("MGRS");
const QString OptionsController::USNG = QStringLiteral("USNG");
const QString OptionsController::GeoRef = QStringLiteral("GEOREF");
const QString OptionsController::Gars = QStringLiteral("GARS");
const QString OptionsController::Meters = QStringLiteral("meters");
const QString OptionsController::Feet = QStringLiteral("feet");

/*
 \brief Constructor that takes an optional \a parent.
 */
OptionsController::OptionsController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);
  m_coordinateFormatOptions << DMS << DD << DDM << UTM << MGRS << USNG << GeoRef << Gars;
  m_units << Meters << Feet;
  emit unitsChanged();
  emit coordinateFormatsChanged();
}

/*
 \brief Destructor
 */
OptionsController::~OptionsController()
{
}

/*
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

/*
 \brief Returns the tool name.
 */
QString OptionsController::toolName() const
{
  return "Options Tool";
}

/*
 \brief Sets \a properties from the configuration file
 */
void OptionsController::setProperties(const QVariantMap& properties)
{
  // access tool properties from the config
  m_coordinateFormat = properties[COORDINATE_FORMAT_PROPERTYNAME].toString();
  if (m_coordinateFormat.isEmpty())
    m_coordinateFormat = DMS;

  m_unitOfMeasurement = properties[UNIT_OF_MEASUREMENT_PROPERTYNAME].toString();
  if (m_unitOfMeasurement.isEmpty())
    m_unitOfMeasurement = Meters;

  // update properties
  m_initialUnitIndex = m_units.indexOf(m_unitOfMeasurement);
  emit initialUnitIndexChanged();
  m_initialFormatIndex = m_coordinateFormatOptions.indexOf(m_coordinateFormat);
  emit initialFormatIndex();

  // get access to the various tool controllers
  getUpdatedTools();
}

/*
 \brief Returns the coordinate format list for display in the combo box
 */
QStringList OptionsController::coordinateFormats() const
{
  return m_coordinateFormatOptions;
}

/*
 \brief Sets the current coordinate \a format to be used
 */
void OptionsController::setCoordinateFormat(const QString& format)
{
  if (!m_locationTextController)
    return;

  m_locationTextController->setCoordinateFormat(format);
}

/*
 \brief Returns whether to use GPS for elevation for display
 */
bool OptionsController::useGpsForElevation() const
{
  if (!m_locationTextController)
    return false;

  return m_locationTextController->useGpsForElevation();
}

/*
 \brief Sets whether to \a useGps for elevation display
 */
void OptionsController::setUseGpsForElevation(bool useGps)
{
  if (!m_locationTextController)
    return;

  m_locationTextController->setUseGpsForElevation(useGps);
}

/*
 \brief Returns the unit of measurement list for display
 */
QStringList OptionsController::units() const
{
  return m_units;
}

/*
 \brief Sets the \a unit of measurement
 */
void OptionsController::setUnitOfMeasurement(const QString& unit)
{
  if (!m_locationTextController)
    return;

  m_locationTextController->setUnitOfMeasurement(unit);
}

/*
 \brief Returns the initial index.
  This is used to set the initial index in the combo box to match the controller
  */
int OptionsController::initialFormatIndex() const
{
  return m_initialFormatIndex;
}

/*
 \brief Returns the initial index.
  This is used to set the initial index in the combo box to match the controller
  */
int OptionsController::initialUnitIndex() const
{
  return m_initialUnitIndex;
}

/*
 \brief Returns whether the friendly tracks labels show.
*/
bool OptionsController::showFriendlyTracksLabels()
{
  auto renderer = friendlyOverlayRenderer();
  if (!renderer)
    return true;

  return renderer->isTextVisible();
}

/*
 \brief Sets whether the friendly tracks labels \a show.
*/
void OptionsController::setShowFriendlyTracksLabels(bool show)
{
  auto renderer = friendlyOverlayRenderer();
  if (!renderer)
    return;

  renderer->setTextVisible(show);
}

/*
 \brief Returns the DictionaryRenderer from the friendly tracks MessageFeed.
*/
DictionaryRenderer* OptionsController::friendlyOverlayRenderer()
{
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
        if (QString(feed->feedName()).toLower() == "friendly tracks")
        {
          Renderer* renderer = feed->messagesOverlay()->renderer();
          if (renderer)
            return dynamic_cast<DictionaryRenderer*>(renderer);
        }
      }
    }
  }
  return nullptr;
}
