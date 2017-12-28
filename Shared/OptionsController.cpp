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
#include "LocationController.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

OptionsController::OptionsController(QObject* parent):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  for (auto it = Toolkit::ToolManager::instance().begin(); it!=Toolkit::ToolManager::instance().end(); ++it)
  {
    if (!it.value())
      continue;

    m_locationController = dynamic_cast<LocationController*>(it.value());
    if (m_locationController)
    {
      m_simulateLocation = m_locationController->isSimulated();
      emit simulateLocationChanged();

      m_locationControllerReady = true;
      emit locationControllerReadyChanged();

      connect(m_locationController, &LocationController::simulatedChanged, [this]
      {
        m_simulateLocation = m_locationController->isSimulated();
        emit simulateLocationChanged();
      });

      return;
    }
  }

  //  connect(Toolkit::ToolManager::instance(),
  //          &Toolkit::ToolManager::instance().toolListChanged, [this]
  //  {
  //  });
}

OptionsController::~OptionsController()
{
}

QString OptionsController::toolName() const
{
  return "Options Tool";
}

void OptionsController::setSimulateLocation(bool simulate)
{
  m_locationController->setSimulated(simulate);
}
