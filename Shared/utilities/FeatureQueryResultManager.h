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

#ifndef FEATUREQUERYRESULTSMANAGER_H
#define FEATUREQUERYRESULTSMANAGER_H

// Qt headers
#include <QList>

namespace Esri::ArcGISRuntime {
  class FeatureQueryResult;
}

namespace Dsa {

struct FeatureQueryResultManager {

  Esri::ArcGISRuntime::FeatureQueryResult* m_results;

  FeatureQueryResultManager(Esri::ArcGISRuntime::FeatureQueryResult* results);

  ~FeatureQueryResultManager();
};

} // Dsa

#endif // FEATUREQUERYRESULTSMANAGER_H
