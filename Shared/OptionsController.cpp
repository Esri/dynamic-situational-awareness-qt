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

using namespace Esri::ArcGISRuntime;

OptionsController::OptionsController(QObject* parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  // get access to the various tool controllers
  getUpdatedTools();
}

OptionsController::~OptionsController()
{
}

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

    connect(m_locationTextController, &LocationTextController::coordinateFormatChanged, this, [this]
    {
      m_initialFormatIndex = m_locationTextController->coordinateFormatOptions().indexOf(m_locationTextController->coordinateFormat());
      emit initialFormatIndex();
    });

    connect(m_locationTextController, &LocationTextController::unitOfMeasurementChanged, this, [this]
    {
      m_initialUnitIndex = m_locationTextController->units().indexOf(m_locationTextController->unitOfMeasurement());
      emit initialUnitIndexChanged();
    });

    emit coordinateFormatsChanged();
    emit useGpsForElevationChanged();
    emit unitsChanged();
  }
}

QString OptionsController::toolName() const
{
  return "Options Tool";
}

// Getter to return the coordinate format list for display in the combo box
QStringList OptionsController::coordinateFormats() const
{
  if (m_locationTextController)
    return m_locationTextController->coordinateFormatOptions();
  else
    return QStringList{};
}

// Setter to set the current coordinate format to be used
void OptionsController::setCoordinateFormat(const QString& format)
{
  if (m_locationTextController)
    m_locationTextController->setCoordinateFormat(format);
}

// Getter to return whether to use GPS for elevation for display in a check box
bool OptionsController::useGpsForElevation() const
{
  if (m_locationTextController)
    return m_locationTextController->useGpsForElevation();
  else
    return false;
}

// Setter to set whether to use GPS for elevation for display
void OptionsController::setUseGpsForElevation(bool useGps)
{
  if (m_locationTextController)
    m_locationTextController->setUseGpsForElevation(useGps);
}

// Getter to return the unit of measurement list for display in the combo box
QStringList OptionsController::units() const
{
  if (m_locationTextController)
    return m_locationTextController->units();
  else
    return QStringList{};
}

// Setter to set the unit of measurement
void OptionsController::setUnitOfMeasurement(const QString& unit)
{
  if (m_locationTextController)
    m_locationTextController->setUnitOfMeasurement(unit);
}

// Getter for initial index. This is used to set the initial index in the combo box to match the controller
int OptionsController::initialFormatIndex() const
{
  return m_initialFormatIndex;
}

// Getter for initial index. This is used to set the initial index in the combo box to match the controller
int OptionsController::initialUnitIndex() const
{
  return m_initialUnitIndex;
}
