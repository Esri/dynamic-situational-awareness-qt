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

// PCH header
#include "pch.hpp"

#include "GraphicsOverlaysResultsManager.h"

// C++ API headers
#include "IdentifyGraphicsOverlayResult.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

// RAII helper to ensure the QList<IdentifyGraphicsOverlayResult*> is deleted when we leave the scope
GraphicsOverlaysResultsManager::GraphicsOverlaysResultsManager(const QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*>& results):
  m_results(results)
{

}

GraphicsOverlaysResultsManager::~GraphicsOverlaysResultsManager()
{
  qDeleteAll(m_results);
}

} // Dsa
