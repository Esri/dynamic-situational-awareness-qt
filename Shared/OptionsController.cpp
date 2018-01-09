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
  for (auto it = Toolkit::ToolManager::instance().begin(); it != Toolkit::ToolManager::instance().end(); ++it)
  {
    if (!it.value())
      continue;

//    m_locationController = dynamic_cast<LocationController*>(it.value());
//    if (m_locationController)
//    {
//      emit simulateLocationChanged();
//      emit locationControllerReadyChanged();

//      connect(m_locationController, &LocationController::simulatedChanged, [this]
//      {
//        emit simulateLocationChanged();
//      });

//      break;
//    }
  }
}

//bool OptionsController::locationControllerReady()
//{
//  if (m_locationController != nullptr)
//    return true;
//  else
//    return false;
//}

//bool OptionsController::simulateLocation()
//{
//  if (m_locationController)
//    return m_locationController->isSimulated();
//  else
//    return false;
//}

QString OptionsController::toolName() const
{
  return "Options Tool";
}

//void OptionsController::setSimulateLocation(bool simulate)
//{
//  if (!m_locationController)
//    return;

// if (m_locationController->isSimulated() == simulate)
//    return;

// m_locationController->setSimulated(simulate);
//}

//QString OptionsController::simulationFile() const
//{
//  if (m_locationController)
//    return m_locationController->gpxFilePathAsString();
//  else
//    return "";
//}

//void OptionsController::setSimulationFile(const QString& file)
//{
//  if (!m_locationController)
//    return;

//  if (m_locationController->gpxFilePathAsString() == file)
//    return;

//  m_locationController->setGpxFilePath(QUrl(file));
//}

