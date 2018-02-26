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

#include "LineOfSightController.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "AnalysisOverlay.h"
#include "GeoView.h"
#include "GeometryEngine.h"
#include "FeatureLayer.h"
#include "LayerListModel.h"
#include "LocationLineOfSight.h"
#include "SceneView.h"

#include <QDebug>
#include <QStringListModel>

using namespace Esri::ArcGISRuntime;

// RAII helper to ensure the FeatureQueryResult* is deleted when we leave the scope
struct FeatureQueryResultManager {

  FeatureQueryResult* m_results;

  FeatureQueryResultManager(FeatureQueryResult* results):
    m_results(results)
  {
  }

  ~FeatureQueryResultManager()
  {
    delete m_results;
  }
};

LineOfSightController::LineOfSightController(QObject* parent):
  Toolkit::AbstractTool(parent),
  m_overlayNames(new QStringListModel(this)),
  m_lineOfSightOverlay(new AnalysisOverlay(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  auto resourecProvider = Toolkit::ToolResourceProvider::instance();
  connect(resourecProvider, &Toolkit::ToolResourceProvider::geoViewChanged, this, [this]()
  {
    onGeoViewChanged(Toolkit::ToolResourceProvider::instance()->geoView());
  });
  connect(resourecProvider, &Toolkit::ToolResourceProvider::sceneChanged, this, [this]()
  {
    onOperationalLayersChanged(Toolkit::ToolResourceProvider::instance()->operationalLayers());
  });

  connect(resourecProvider, &Toolkit::ToolResourceProvider::locationChanged, this, &LineOfSightController::onLocationChanged);
}

LineOfSightController::~LineOfSightController()
{
  cancelTask();
}

QString LineOfSightController::toolName() const
{
  return QStringLiteral("Line of sight");
}

void LineOfSightController::setProperties(const QVariantMap& properties)
{
}

void LineOfSightController::setActive(bool active)
{
  if (m_active == active)
    return;

  if (m_active)
    cancelTask();

  AbstractTool::setActive(active);
}

void LineOfSightController::onGeoViewChanged(GeoView* geoView)
{
  if (m_geoView == geoView)
    return;

  m_geoView = geoView;
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  sceneView->analysisOverlays()->append(m_lineOfSightOverlay);
}

void LineOfSightController::onOperationalLayersChanged(LayerListModel* operationalLayers)
{
  if (!operationalLayers)
  {
    m_overlayNames->setStringList(QStringList());
    emit overlayNamesChanged();

    return;
  }

  auto updateNames = [this, operationalLayers]()
  {
    QStringList overlayNames;
    m_overlays.clear();

    if (operationalLayers)
    {
      const int opLayersCount = operationalLayers->rowCount();
      for (int i = 0; i < opLayersCount; ++i)
      {
        Layer* lyr = operationalLayers->at(i);
        if (!lyr)
          continue;

        FeatureLayer* featLayer = qobject_cast<FeatureLayer*>(lyr);
        if (!featLayer)
          continue;

        if (featLayer->loadStatus() != LoadStatus::Loaded)
        {
          connect(featLayer, &FeatureLayer::doneLoading, this, [this, operationalLayers]()
          {
            onOperationalLayersChanged(operationalLayers);
          });
        }
        else
        {
          overlayNames.append(featLayer->name());
          m_overlays.append(featLayer);
        }
      }
    }

    m_overlayNames->setStringList(overlayNames);
    emit overlayNamesChanged();
  };

  connect(operationalLayers, &LayerListModel::rowsInserted, this, updateNames);
  connect(operationalLayers, &LayerListModel::rowsRemoved, this, updateNames);

  updateNames();
}

void LineOfSightController::onLocationChanged(const Point& location)
{
  if (!isActive())
    return;

  if (m_location == location)
    return;

  m_location = location;

  AnalysisListModel* model = m_lineOfSightOverlay->analyses();
  if (model == nullptr)
    return;

  const auto lineOfSightCount = model->rowCount();
  for (int i = 0; i < lineOfSightCount; ++i)
  {
    Analysis* analysis = model->at(i);
    if (analysis == nullptr)
      continue;

    LocationLineOfSight* lineOfSight = qobject_cast<LocationLineOfSight*>(analysis);
    if (!lineOfSight)
      continue;

    lineOfSight->setObserverLocation(m_location);
  }
}

void LineOfSightController::onQueryFeaturesCompleted(QUuid taskId, FeatureQueryResult* featureQueryResult)
{
  if (taskId != m_featuresTask.taskId())
    return;

  disconnect(m_queryFeaturesConnection);
  m_featuresTask = TaskWatcher();

  FeatureQueryResultManager resultsMgr(featureQueryResult);

  if (m_lineOfSightParent)
  {
    delete m_lineOfSightParent;
    m_lineOfSightParent = nullptr;
  }

  m_lineOfSightParent = new QObject(this);
  QObject localParent;

  QList<Feature*> features = resultsMgr.m_results->iterator().features(&localParent);
  auto it = features.constBegin();
  auto itEnd = features.constEnd();
  for (; it != itEnd; ++it)
  {
    Feature* feat = *it;
    if (feat == nullptr)
      continue;

    const Geometry featGeometry = feat->geometry();
    Point featLocation;

    switch (featGeometry.geometryType())
    {
    case GeometryType::Point:
      featLocation = featGeometry;
      break;
    default:
      featLocation = featLocation.extent().center();
      break;
    }

    const Point featLocationProj = GeometryEngine::instance()->project(featLocation, m_location.spatialReference());

    LocationLineOfSight* lineOfSight = new LocationLineOfSight(m_location, featLocationProj, m_lineOfSightParent);
    lineOfSight->setVisible(m_analysisVisible);
    m_lineOfSightOverlay->analyses()->append(lineOfSight);
  }
}

void LineOfSightController::cancelTask()
{
  m_featuresTask.cancel();
  disconnect(m_queryFeaturesConnection);
}

bool LineOfSightController::analysisVisible() const
{
  return m_analysisVisible;
}

void LineOfSightController::setAnalysisVisible(bool analysisVisible)
{
  if (analysisVisible == m_analysisVisible)
    return;

  m_analysisVisible = analysisVisible;

  AnalysisListModel* model = m_lineOfSightOverlay->analyses();
  if (model == nullptr)
    return;

  const auto lineOfSightCount = model->rowCount();
  for (int i = 0; i < lineOfSightCount; ++i)
  {
    Analysis* analysis = model->at(i);
    if (analysis == nullptr)
      continue;

    LocationLineOfSight* lineOfSight = qobject_cast<LocationLineOfSight*>(analysis);
    if (!lineOfSight)
      continue;

    lineOfSight->setVisible(m_analysisVisible);
  }

  emit analysisVisibleChanged();
}

QAbstractItemModel* LineOfSightController::overlayNames() const
{
  return m_overlayNames;
}

void LineOfSightController::selectOverlayIndex(int selectOverlayIndex)
{
  if (m_featuresTask.isValid() && !m_featuresTask.isCanceled() && !m_featuresTask.isDone())
    cancelTask();

  clearAnalysis();

  if (selectOverlayIndex > m_overlays.size() || selectOverlayIndex == -1)
    return;

  FeatureLayer* overlay = m_overlays.at(selectOverlayIndex);
  if (!overlay)
  {
    emit toolErrorOccurred(QStringLiteral("Invalid Line of sight overlay selected"),
                           QString("The selected overlay index is invalid: %1").arg(QString::number(selectOverlayIndex)));
    return;
  }

  QueryParameters query;
  query.setWhereClause(QStringLiteral("1=1"));
  query.setReturnGeometry(true);
  m_queryFeaturesConnection = connect(overlay->featureTable(), &FeatureTable::queryFeaturesCompleted, this, &LineOfSightController::onQueryFeaturesCompleted);
  m_featuresTask = overlay->featureTable()->queryFeatures(query);
}

void LineOfSightController::clearAnalysis()
{
  m_lineOfSightOverlay->analyses()->clear();
  if (m_lineOfSightParent)
  {
    delete m_lineOfSightParent;
    m_lineOfSightParent = nullptr;
  }
}
