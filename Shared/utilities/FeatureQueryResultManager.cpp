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

#include "FeatureQueryResultManager.h"

#include "FeatureQueryResult.h"

using namespace Esri::ArcGISRuntime;

/*!
  \struct FeatureQueryResultManager
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
