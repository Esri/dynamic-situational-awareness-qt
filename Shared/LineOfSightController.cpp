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

/*!
  \brief Constructor accepting an optional \a parent.
 */
LineOfSightController::LineOfSightController(QObject* parent):
  Toolkit::AbstractTool(parent),
  m_overlayNames(new QStringListModel(this)),
  m_lineOfSightOverlay(new AnalysisOverlay(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  // connect to ToolResourceProvider signals
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

/*!
  \brief Destructor.
 */
LineOfSightController::~LineOfSightController()
{
  cancelTask();
}

/*!
  \brief Returns the name of this tool.
 */
QString LineOfSightController::toolName() const
{
  return QStringLiteral("Line of sight");
}

/*!
  \brief Sets the tool to be \a active.
 */
void LineOfSightController::setActive(bool active)
{
  if (m_active == active)
    return;

  // if the tool is becoming inactive, cancel any outstanding tasks
  if (m_active)
    cancelTask();

  AbstractTool::setActive(active);
}

/*!
  \brief Handle the new \a geoView.

  Line of sight results will be added to this view.
 */
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

/*!
  \brief Handle the new list of \a operationalLayers.

  Any \l Esri::ArcGISRuntime::FeatureLayer objects will be added to the list of
  \l overlayNames, to be used as the targets for Line of Sight analysis.
 */
void LineOfSightController::onOperationalLayersChanged(LayerListModel* operationalLayers)
{
  // if there are no layers, clear the overlays list
  if (!operationalLayers)
  {
    m_overlayNames->setStringList(QStringList());
    emit overlayNamesChanged();

    return;
  }

  // lambda to update the list of overlays
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

        // if the feature layer is not loaded, react to the loaded signal and update the list
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

  // update the list of overlays whenever layers are added/removed
  connect(operationalLayers, &LayerListModel::rowsInserted, this, updateNames);
  connect(operationalLayers, &LayerListModel::rowsRemoved, this, updateNames);

  // call the lambda to set the initial list of overlay names
  updateNames();
}

/*!
  \brief Handle the new \a location.

  This will be the observer position for Line of Sight analysis.
 */
void LineOfSightController::onLocationChanged(const Point& location)
{
  // if the tool is not active, ignore
  if (!isActive())
    return;

  if (m_location == location)
    return;

  m_location = location;

  AnalysisListModel* model = m_lineOfSightOverlay->analyses();
  if (model == nullptr)
    return;

  // update any existing Line of sight results with the new position
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

/*!
  \brief Handle the set of features in \a featureQueryResult returned by task \a taskId.

  This query is the set of all features within the selected overlay. The returned features will
  be used as the targets for Line of Sight analysis.
 */
void LineOfSightController::onQueryFeaturesCompleted(QUuid taskId, FeatureQueryResult* featureQueryResult)
{
  // if the task ID does not match, ignore this result
  if (taskId != m_featuresTask.taskId())
    return;

  // reset query/task tracking
  disconnect(m_queryFeaturesConnection);
  m_featuresTask = TaskWatcher();

  FeatureQueryResultManager resultsMgr(featureQueryResult);

  // clear the QObject used as a parent for Line of Sight results
  if (m_lineOfSightParent)
  {
    delete m_lineOfSightParent;
    m_lineOfSightParent = nullptr;
  }
  m_lineOfSightParent = new QObject(this);

  // create a local QObject to as as the parent for returned features
  // These are only required within the scope of this method
  QObject localParent;

  // For each feature, obtain a point location and use it as the target for a new
  // LocationLineOfSight which will be added to the overlay.
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

/*!
  \brief Internal.

  Cancel the current feature query task.
 */
void LineOfSightController::cancelTask()
{
  m_featuresTask.cancel();
  disconnect(m_queryFeaturesConnection);
}

/*!
  \brief Returns the tool's Line of Sight Overlay.
 */
AnalysisOverlay* LineOfSightController::lineOfSightOverlay() const
{
  return m_lineOfSightOverlay;
}

/*!
  \brief Returns whether the results of Line of sight analysis should be visible.
 */
bool LineOfSightController::analysisVisible() const
{
  return m_analysisVisible;
}

/*!
  \brief Sets whether the results of Line of sight analysis should be visible to \a analysisVisible.
 */
void LineOfSightController::setAnalysisVisible(bool analysisVisible)
{
  if (analysisVisible == m_analysisVisible)
    return;

  m_analysisVisible = analysisVisible;

  AnalysisListModel* model = m_lineOfSightOverlay->analyses();
  if (model == nullptr)
    return;

  // update the visible state of any existing results
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

/*!
  \brief Returns the list of overlay names which are suitable for Line of sight analysis.
 */
QAbstractItemModel* LineOfSightController::overlayNames() const
{
  return m_overlayNames;
}

/*!
  \brief Select the overlay index (\a selectOverlayIndex) to use for Line of sight analysis.

  The index refers to the overalys returned by \l overlayNames.
 */
void LineOfSightController::selectOverlayIndex(int selectOverlayIndex)
{
  // cancel any query tasks which are currently running
  if (m_featuresTask.isValid() && !m_featuresTask.isCanceled() && !m_featuresTask.isDone())
    cancelTask();

  // clear the results of any existing analysis
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

  // perform a query to retrieve all the features from the selected overlay. These will be the target features for Line of sight analysis.
  QueryParameters query;
  query.setWhereClause(QStringLiteral("1=1"));
  query.setReturnGeometry(true);
  m_queryFeaturesConnection = connect(overlay->featureTable(), &FeatureTable::queryFeaturesCompleted, this, &LineOfSightController::onQueryFeaturesCompleted);
  m_featuresTask = overlay->featureTable()->queryFeatures(query);
}

/*!
  \brief Clears the current Line of sight analysis.
 */
void LineOfSightController::clearAnalysis()
{
  // remove all of the results from the overlay
  m_lineOfSightOverlay->analyses()->clear();
  // delete the QObject used as the parent for the analysis
  if (m_lineOfSightParent)
  {
    delete m_lineOfSightParent;
    m_lineOfSightParent = nullptr;
  }
}
