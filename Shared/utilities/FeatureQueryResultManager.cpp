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

#include "FeatureQueryResultManager.h"

using namespace Esri::ArcGISRuntime;

// RAII helper to ensure the QList<IdentifyLayerResult*> is deleted when we leave the scope
FeatureQueryResultManager::FeatureQueryResultManager(FeatureQueryResult* results):
  m_results(results)
{

}

FeatureQueryResultManager::~FeatureQueryResultManager()
{
  delete m_results;
}
