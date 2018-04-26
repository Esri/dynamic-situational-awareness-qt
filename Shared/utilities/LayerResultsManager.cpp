/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

// PCH header
#include "pch.hpp"

#include "LayerResultsManager.h"

// C++ API headers
#include "IdentifyLayerResult.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::LayerResultsManager
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
