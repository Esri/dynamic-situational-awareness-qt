// Copyright 2016 ESRI
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

// API
#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "ElevationSource.h"
#include "Layer.h"
#include "GeoView.h"
#include "DictionarySymbolStyle.h"

// Toolkit
#include "AbstractTool.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

// Dsa apps
#include "DsaUtility.h"
#include "DsaController.h"

using namespace Esri::ArcGISRuntime;

DsaController::DsaController(QObject* parent):
  QObject(parent),
  m_scene(new Scene(this)),
  m_dataPath(DsaUtility::dataPath())
{
  // Set viewpoint to Monterey, CA
  // distance of 5000m, heading North, pitch at 75 degrees, roll of 0
  Camera monterey(DsaUtility::montereyCA(), 5000, 0., 75., 0);
  Viewpoint initViewpoint(DsaUtility::montereyCA(), monterey);
  m_scene->setInitialViewpoint(initViewpoint);

  connect(m_scene, &Scene::errorOccurred, this, &DsaController::onError);

  // set an elevation source
  ArcGISTiledElevationSource* source = new ArcGISTiledElevationSource(QUrl(m_dataPath + "/elevation/CaDEM.tpk"), this);
  connect(source, &ArcGISTiledElevationSource::errorOccurred, this, &DsaController::onError);
  m_scene->baseSurface()->elevationSources()->append(source);
}

DsaController::~DsaController()
{
}

Esri::ArcGISRuntime::Scene* DsaController::scene() const
{
  return m_scene;
}

void DsaController::init(GeoView* geoView)
{
  Toolkit::ToolManager::instance()->resourceProvider()->registerScene(m_scene);
  Toolkit::ToolManager::instance()->resourceProvider()->registerGeoView(geoView);

  auto toolsIt = Toolkit::ToolManager::instance()->toolsBegin();
  auto toolsEnd = Toolkit::ToolManager::instance()->toolsEnd();
  for( ; toolsIt != toolsEnd; ++toolsIt)
  {
    Toolkit::AbstractTool* abstractTool = *toolsIt;
    if (!abstractTool)
      continue;

    connect(abstractTool, &Toolkit::AbstractTool::errorOccurred, this, &DsaController::onError);
  }
}

void DsaController::onError(const Esri::ArcGISRuntime::Error& e)
{
  qDebug() << "Error" << e.message() << e.additionalMessage();
}
