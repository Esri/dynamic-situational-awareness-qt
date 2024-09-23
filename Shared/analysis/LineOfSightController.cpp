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

#include "LineOfSightController.h"

// C++ API headers
#include "AnalysisListModel.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "Error.h"
#include "Feature.h"
#include "FeatureIterator.h"
#include "FeatureLayer.h"
#include "FeatureQueryResult.h"
#include "FeatureTable.h"
#include "GeoElementLineOfSight.h"
#include "GeoView.h"
#include "Graphic.h"
#include "LayerListModel.h"
#include "MapTypes.h"
#include "QueryParameters.h"
#include "SceneView.h"
#include "SceneViewTypes.h"

// Qt headers
#include <QFuture>
#include <QStringListModel>

// DSA headers
#include "FeatureQueryResultManager.h"
#include "LocationController.h"
#include "LocationDisplay3d.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::LineOfSightController
  \inmodule Dsa
  \inherits AbstractTool
  \brief Tool controller for creating line of sight analysis.

  This tool allows line of sight analysis to be created:

  \list
    \li From the objects in a feature layer to the current position.
    \li From the current position to a supplied GeoElement.
  \endlist
 */

/*!
  \internal
 */
void LineOfSightController::getLocationGeoElement()
{
  LocationController* locationController = ToolManager::instance().tool<LocationController>();
  if (locationController)
    m_locationGeoElement = locationController->locationDisplay()->locationGraphic();
}

/*!
  \property LineOfSightController::visibleByCount
  \brief Returns the number of line of sight analyses from features to the current position
  which are unobstructed.
 */
int LineOfSightController::visibleByCount() const
{
  return m_visibleByCount;
}

/*!
  \internal
 */
void LineOfSightController::setVisibleByCount(int visibleByCount)
{
  if (m_visibleByCount == visibleByCount)
    return;

  m_visibleByCount = visibleByCount;
  emit visibleByCountChanged();
}

/*!
  \brief Constructor accepting an optional \a parent.
 */
LineOfSightController::LineOfSightController(QObject* parent):
  AbstractTool(parent),
  m_overlayNames(new QStringListModel(this)),
  m_lineOfSightOverlay(new AnalysisOverlay(this))
{
  // connect to ToolResourceProvider signals
  auto resourecProvider = ToolResourceProvider::instance();
  connect(resourecProvider, &ToolResourceProvider::geoViewChanged, this, [this]()
  {
    onGeoViewChanged(ToolResourceProvider::instance()->geoView());
  });
  connect(resourecProvider, &ToolResourceProvider::sceneChanged, this, [this]()
  {
    onOperationalLayersChanged();

    // this tool must be in the tool manager before adding analyses below
    ToolManager::instance().addTool(this);

    SceneView* sceneView = dynamic_cast<SceneView*>(ToolResourceProvider::instance()->geoView());
    if (!sceneView)
      return;

    if (m_lineOfSightOverlay)
    {
      delete m_lineOfSightOverlay;
      m_lineOfSightOverlay = new AnalysisOverlay(this);
    }

    m_geoView = sceneView;
    sceneView->analysisOverlays()->append(m_lineOfSightOverlay);
  });

  onOperationalLayersChanged();

  // this tool must be in the tool manager before adding analyses below
  ToolManager::instance().addTool(this);

  SceneView* sceneView = dynamic_cast<SceneView*>(ToolResourceProvider::instance()->geoView());
  if (sceneView)
  {
    m_geoView = sceneView;
    sceneView->analysisOverlays()->append(m_lineOfSightOverlay);
  }
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
  SceneView* sceneView = dynamic_cast<SceneView*>(ToolResourceProvider::instance()->geoView());
  if (!sceneView)
    return;

  sceneView->analysisOverlays()->append(m_lineOfSightOverlay);
}

/*!
  \brief Handle the new list of \a operationalLayers.

  Any \l Esri::ArcGISRuntime::FeatureLayer objects will be added to the list of
  \l overlayNames, to be used as the targets for Line of Sight analysis.
 */
void LineOfSightController::onOperationalLayersChanged()
{
  Esri::ArcGISRuntime::LayerListModel* operationalLayers = ToolResourceProvider::instance()->operationalLayers();
  // if there are no layers, clear the overlays list
  if (!operationalLayers)
  {
    m_overlayNames->setStringList(QStringList());
    emit overlayNamesChanged();

    return;
  }

  // lambda to update the list of overlays
  auto updateNames = [this]()
  {
    QStringList overlayNames;
    m_overlays.clear();

    Esri::ArcGISRuntime::LayerListModel* operationalLayers = ToolResourceProvider::instance()->operationalLayers();
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
          connect(featLayer, &FeatureLayer::doneLoading, this, [this](Error e)
          {
            if (!e.isEmpty())
            {
              emit errorOccurred(e);
              return;
            }

            onOperationalLayersChanged();
          });
        }
        else
        {
          // Only point layers are suitable for Line of sight
          if (featLayer->featureTable()->geometryType() == GeometryType::Point)
          {
            overlayNames.append(featLayer->name());
            m_overlays.append(featLayer);
          }
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
  \brief Internal.

  Cancel the current feature query task.
 */
void LineOfSightController::cancelTask()
{
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
  \brief Creates a new Line of sight from the app's current location to the target \a geoElement.
 */
void LineOfSightController::lineOfSightFromLocationToGeoElement(GeoElement* geoElement)
{
  if (!geoElement)
  {
    emit toolErrorOccurred(QStringLiteral("Invalid Line Of Sight target"), QStringLiteral("Null GeoElement"));
    return;
  }

  if (!m_locationGeoElement)
    getLocationGeoElement();

  if (!m_locationGeoElement)
  {
    emit toolErrorOccurred(QStringLiteral("Failed to get location"), QStringLiteral("Unable to find My Location GeoElement for GeoElementLineOfSight"));
    return;
  }

  // create a Line of sight from the feature to the current location
  GeoElementLineOfSight* lineOfSight = new GeoElementLineOfSight(m_locationGeoElement, geoElement, m_lineOfSightParent);
  lineOfSight->setVisible(true);
  m_lineOfSightOverlay->analyses()->append(lineOfSight);
}

/*!
  \property LineOfSightController::analysisVisible
  \brief Returns whether the results of Line of sight analysis should be visible.
 */
bool LineOfSightController::isAnalysisVisible() const
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

    GeoElementLineOfSight* lineOfSight = qobject_cast<GeoElementLineOfSight*>(analysis);
    if (!lineOfSight)
      continue;

    lineOfSight->setVisible(m_analysisVisible);
  }

  emit analysisVisibleChanged();
}

/*!
  \property Dsa::LineOfSightController::overlayNames
  \brief Returns the list of overlay names which are suitable for Line of sight analysis.
 */
QAbstractItemModel* LineOfSightController::overlayNames() const
{
  return m_overlayNames;
}

/*!
  \brief Select the overlay index (\a selectOverlayIndex) to use for Line of sight analysis.

  The index refers to the overalys returned by \l overlayNames.

  Returns whether the overlay was succesfully used to perform analysis.
 */
void LineOfSightController::selectOverlayIndex(int selectOverlayIndex)
{
  // clear the results of any existing analysis
  clearAnalysis();

  if (selectOverlayIndex > m_overlays.size() || selectOverlayIndex == -1)
    emit selectOverlayFailed();
    return;

  FeatureLayer* overlay = m_overlays.at(selectOverlayIndex);
  if (!overlay)
  {
    emit toolErrorOccurred(QStringLiteral("Invalid Line of sight overlay selected"),
                           QString("The selected overlay index is invalid: %1").arg(QString::number(selectOverlayIndex)));
    emit selectOverlayFailed();
    return;
  }

  constexpr int maxFeatures = 16; // Due to performance reasons, limit the number of features which can be used in the analysis
  const int featuresCount = overlay->featureTable()->numberOfFeatures();
  if (featuresCount > maxFeatures)
  {
    emit toolErrorOccurred(QString("There are too many points in this layer (%1).\n Please choose another one with %2 or fewer points.")
                           .arg(QString::number(featuresCount), QString::number(maxFeatures)),
                           QStringLiteral("For performance reasons, Line of Sight analysis is limited to a maximum number of features"));
    emit selectOverlayFailed();
    return;
  }

  // perform a query to retrieve all the features from the selected overlay. These will be the target features for Line of sight analysis.
  QueryParameters query;
  query.setWhereClause(QStringLiteral("1=1"));
  query.setReturnGeometry(true);
  overlay->featureTable()->queryFeaturesAsync(query, this).then(this, [this](FeatureQueryResult* results) -> bool
  {
    FeatureQueryResultManager resultsManager{results};

    if (!m_locationGeoElement)
      getLocationGeoElement();

    if (!m_locationGeoElement)
    {
      emit toolErrorOccurred(QStringLiteral("Failed to get location"), QStringLiteral("Unable to find My Location GeoElement for GeoElementLineOfSight"));
      return false;
    }

    for (const auto& conn : m_visibleByConnections)
      disconnect(conn);

    m_visibleByConnections.clear();
    setVisibleByCount(0);

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

    // For each feature, obtain a point location and use it as the observer for a new
    // GeoElementLineOfSight which will be added to the overlay.
    for (auto* feature : resultsManager.m_results->iterator().features(&localParent))
    {
      if (feature == nullptr)
        continue;

      // create a Line of sight from the feature to the current location
      auto* lineOfSight = new GeoElementLineOfSight(feature, m_locationGeoElement, m_lineOfSightParent);
      lineOfSight->setVisible(m_analysisVisible);
      m_lineOfSightOverlay->analyses()->append(lineOfSight);

      m_visibleByConnections.append(connect(lineOfSight, &GeoElementLineOfSight::targetVisibilityChanged, this, [this]()
      {
        int visibleCount = 0;
        auto* losList = m_lineOfSightOverlay->analyses();
        const int count = losList->rowCount();
        for (int i = 0; i < count; ++i)
        {
          auto* analysis = losList->at(i);
          if (!analysis)
            continue;

          auto* lineOfSight = qobject_cast<GeoElementLineOfSight*>(analysis);
          if (!lineOfSight)
            continue;

          // don't count Line of Sight from the current location
          if (lineOfSight->observerGeoElement() == m_locationGeoElement)
            continue;

          if (lineOfSight->targetVisibility() == LineOfSightTargetVisibility::Visible)
            visibleCount += 1;
        }

        setVisibleByCount(visibleCount);
      }));
    }

    // if no early returns or failures happened pass success=true along to the continuation
    return true;
  }).then(this, [this](bool success)
  {
    if (!success)
      emit selectOverlayFailed();
  });
}

/*!
  \brief Clears the current Line of sight analysis.
 */
void LineOfSightController::clearAnalysis()
{
  // remove all of the results from the overlay
  m_lineOfSightOverlay->analyses()->clear();

  for (const auto& conn : m_visibleByConnections)
    disconnect(conn);

  m_visibleByConnections.clear();
  setVisibleByCount(0);

  // delete the QObject used as the parent for the analysis
  if (m_lineOfSightParent)
  {
    delete m_lineOfSightParent;
    m_lineOfSightParent = nullptr;
  }
}

} // Dsa

// Signal Documentation
/*!
  \fn void AnalysisListController::visibleByCountChanged();
  \brief Signal emitted when the visibleByCount property changes.
 */

/*!
  \fn void AnalysisListController::analysisVisibleChanged();
  \brief Signal emitted when the analysisVisible property changes.
 */

/*!
  \fn void AnalysisListController::overlayNamesChanged();
  \brief Signal emitted when the overlay names change.
 */

/*!
  \fn void AnalysisListController::toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  \brief Signal emitted when an error occurs.

  An error \a message and \a additionalMessage are passed through as parameters, describing
  the error that occurred.
 */
