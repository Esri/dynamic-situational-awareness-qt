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

#include "pch.hpp"

#include "LayerResultsManager.h"

#include "IdentifyLayerResult.h"

using namespace Esri::ArcGISRuntime;

// RAII helper to ensure the QList<IdentifyLayerResult*> is deleted when we leave the scope
LayerResultsManager::LayerResultsManager(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& results):
  m_results(results)
{

}

LayerResultsManager::~LayerResultsManager()
{
  qDeleteAll(m_results);
}
