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
  m_locationTextController = Toolkit::ToolManager::instance().tool<LocationTextController>();
  if (m_locationTextController)
  {
    m_coordinateFormats = m_locationTextController->coordinateFormatOptions();
    emit coordinateFormatsChanged();
  }
}

QString OptionsController::toolName() const
{
  return "Options Tool";
}

QStringList OptionsController::coordinateFormats() const
{
  return m_coordinateFormats;
}

void OptionsController::setCoordinateFormat(const QString& format)
{
  if (m_locationTextController)
    m_locationTextController->setCoordinateFormat(format);
}
