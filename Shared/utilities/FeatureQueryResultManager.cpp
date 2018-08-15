
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

#include "FeatureQueryResultManager.h"

// C++ API headers
#include "FeatureQueryResult.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::FeatureQueryResultManager
  \inmodule Dsa
  \brief RAII helper to ensure the \l Esri::ArcGISRuntime::FeatureQueryResult*
  is deleted when we leave the scope.
 */

/*!
  \brief Constructor taking an \l Esri::ArcGISRuntime::FeatureQueryResult \a results.
 */
FeatureQueryResultManager::FeatureQueryResultManager(FeatureQueryResult* results):
  m_results(results)
{
}

/*!
  \brief Destructor.
 */
FeatureQueryResultManager::~FeatureQueryResultManager()
{
  delete m_results;
}

} // Dsa
