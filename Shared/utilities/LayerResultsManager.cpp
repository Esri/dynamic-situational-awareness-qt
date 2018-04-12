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

#include "LayerResultsManager.h"

// C++ API headers
#include "IdentifyLayerResult.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \struct FeatureQueryResultManager
  \brief RAII helper to ensure the list of \l Esri::ArcGISRuntime::IdentifyLayerResult* objects
  is deleted when we leave the scope.
 */

/*!
  \brief Constructor taking a list of  \l Esri::ArcGISRuntime::IdentifyLayerResult \a results.
 */
LayerResultsManager::LayerResultsManager(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& results):
  m_results(results)
{

}

/*!
  \brief Destructor.
 */
LayerResultsManager::~LayerResultsManager()
{
  qDeleteAll(m_results);
}

} // Dsa
