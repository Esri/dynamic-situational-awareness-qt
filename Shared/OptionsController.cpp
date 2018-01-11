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

    /*
     * // 1) cast each to the type you need, and store as a member
     * e.g. m_locationController = dynamic_cast<LocationController*>(it.value());
     *      if (m_locationController) ...
     * // 2) wrap properties and expose as Q_PROPERTY in the options tool
     * e.g. emit simulateLocationChanged(); // bool that returns m_locationController->isSimulated();. This is exposed as a checkbox in QML
    */
  }
}

QString OptionsController::toolName() const
{
  return "Options Tool";
}
