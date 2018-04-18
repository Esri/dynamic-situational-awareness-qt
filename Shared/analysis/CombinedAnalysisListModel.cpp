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

#include "CombinedAnalysisListModel.h"

// example app headers
#include "Viewshed360.h"
#include "ViewshedListModel.h"

// C++ API headers
#include "AnalysisListModel.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "GeoElementLineOfSight.h"
#include "GeoElementViewshed.h"
#include "LocationLineOfSight.h"
#include "LocationViewshed.h"
#include "SceneView.h"
#include "Viewshed.h"

using namespace Esri::ArcGISRuntime;

namespace Dsa {

/*!
  \class Dsa::CombinedAnalysisListModel
  \inmodule Dsa
  \inherits QAbstractListModel
  \brief A Model which manages the list of both line of sight and viewshed analyses.

  \sa Esri::ArcGISRuntime::AnalysisListModel
  \sa ViewshedListModel

  The model returns data for the following roles:
  \table
    \header
        \li Role
        \li Type
        \li Description
    \row
        \li analysisName
        \li QString
        \li The name of the analysis.
    \row
        \li analysisType
        \li int
        \li The type of analysis
    \row
        \li analysisVisible
        \li bool
        \li Whether the analysis is visible.
  \endtable
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
CombinedAnalysisListModel::CombinedAnalysisListModel(QObject* parent):
  QAbstractListModel(parent)
{
  // setup the role names for accessing data i the model from a QQuick view
  m_roles[AnalysisNameRole] = "analysisName";
  m_roles[AnalysisVisibleRole] = "analysisVisible";
  m_roles[AnalysisTypeRole] = "analysisType";
}

/*!
  \brief Destructor.
 */
CombinedAnalysisListModel::~CombinedAnalysisListModel()
{
}

/*!
  \brief Sets the \l ViewshedListModel used by this tool to \a viewshedModel.
 */
void CombinedAnalysisListModel::setViewshedModel(QAbstractItemModel* viewshedModel)
{
  if (viewshedModel == nullptr)
    return;

  auto castModel = qobject_cast<ViewshedListModel*>(viewshedModel);
  if (castModel == nullptr)
    return;

  if (m_viewshedModel == castModel)
    return;

  beginResetModel();
  m_viewshedModel = castModel;
  connectAnalysisListModelSignals(m_viewshedModel);
  endResetModel();
}

/*!
  \brief Sets the \l Esri::ArcGISRuntime::AnalysisListModel used by this tool to \a lineOfSightModel.
 */
void CombinedAnalysisListModel::setLineOfSightModel(AnalysisListModel* lineOfSightModel)
{
  if (m_lineOfSightModel == lineOfSightModel)
    return;

  beginResetModel();
  m_lineOfSightModel = lineOfSightModel;

  // persist a unique index for each Line of sight as they are added - to be used to construct a name
  connect(m_lineOfSightModel, &AnalysisListModel::analysisAdded, this, [this](int index)
  {
    Analysis* addedAnalysis = m_lineOfSightModel->at(index);
    if (addedAnalysis)
      m_lineOfSightIndices.insert(addedAnalysis, m_lineOfSightIndices.count() + 1);
  });

  connectAnalysisListModelSignals(m_lineOfSightModel);
  endResetModel();
}

/*!
  \brief Removes the analysis at \a index from the combined list.
 */
void CombinedAnalysisListModel::removeAt(int index)
{
  if (isViewshed(index))
    m_viewshedModel->removeAt(viewshedIndex(index));
  else if (isLineOfSight(index))
    m_lineOfSightModel->removeAt(lineOfSightIndex(index));
}

/*!
  \brief Returns the location used by the analysis at \a index.
 */
Point CombinedAnalysisListModel::locationAt(int index)
{
  Analysis* analysis = nullptr;
  if (isViewshed(index))
  {
    Viewshed360* viewshed360 =  m_viewshedModel->at(viewshedIndex(index));
    if (viewshed360 != nullptr)
      analysis = viewshed360->viewshed();
  }
  else if (isLineOfSight(index))
  {
    analysis = m_lineOfSightModel->at(lineOfSightIndex(index));
  }

  if (analysis == nullptr)
    return Point();

  switch (analysis->analysisType())
  {
  case AnalysisType::LocationViewshed:
  {
    LocationViewshed* locationViewshed = qobject_cast<LocationViewshed*>(analysis);
    if (locationViewshed == nullptr)
      return Point();

    return locationViewshed->location();
  }
  case AnalysisType::LocationLineOfSight:
  {
    LocationLineOfSight* locationLineOfSight = qobject_cast<LocationLineOfSight*>(analysis);
    if (locationLineOfSight == nullptr)
      return Point();

    return locationLineOfSight->targetLocation();
  }
  case AnalysisType::GeoElementViewshed:
  {
    GeoElementViewshed* geoElementViewshed = qobject_cast<GeoElementViewshed*>(analysis);
    if (geoElementViewshed == nullptr)
      return Point();

    return geoElementViewshed->geoElement()->geometry().extent().center();
  }
  case AnalysisType::GeoElementLineOfSight:
  {
    GeoElementLineOfSight* geoElementLineOfSight = qobject_cast<GeoElementLineOfSight*>(analysis);
    if (geoElementLineOfSight == nullptr)
      return Point();

    return geoElementLineOfSight->targetGeoElement()->geometry().extent().center();
  }
  default:
    return Point();
  }
}

/*!
  \brief Returns the number of rows in the combined list.
 */
int CombinedAnalysisListModel::rowCount(const QModelIndex&) const
{
  return viewshedCount() + lineOfSightCount();
}

/*!
  \brief Returns the data stored in the combined list at \a index under the \l CombinedAnalysisRoles \a role.
 */
QVariant CombinedAnalysisListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return QVariant();

  if (isViewshed(index.row())) // returns the specified role data for a viewshed
  {
    switch (role)
    {
    case AnalysisNameRole:
      return m_viewshedModel->data(m_viewshedModel->index(viewshedIndex(index.row()), 0), ViewshedListModel::ViewshedRoles::ViewshedNameRole);
    case AnalysisVisibleRole:
      return m_viewshedModel->data(m_viewshedModel->index(viewshedIndex(index.row()), 0), ViewshedListModel::ViewshedRoles::ViewshedVisibleRole);
    case AnalysisTypeRole:
      return QStringLiteral("viewshed");
    default:
      break;
    }
  }
  else if (isLineOfSight(index.row())) // returns the specified role data for a line of sight
  {
    switch (role)
    {
    case AnalysisNameRole:
    {
      Analysis* losPtr = m_lineOfSightModel->at(lineOfSightIndex(index.row()));
      if (losPtr)
        return QString("Line of sight %1").arg(m_lineOfSightIndices.value(losPtr));
    }
    case AnalysisVisibleRole:
      return m_lineOfSightModel->data(m_lineOfSightModel->index(lineOfSightIndex(index.row()), 0), AnalysisListModel::AnalysisRoles::AnalysisVisibleRole);
    case AnalysisTypeRole:
      return QStringLiteral("lineOfSight");
    default:
      break;
    }
  }

  return QVariant();
}

/*!
  \brief Sets the data stored in the combined list at \a index under the \l CombinedAnalysisRoles \a role to \a value.

  Returns \c true on success, else \c false.

  \note The  only editable role is \c CombinedAnalysisRoles::AnalysisVisibleRole.
 */
bool CombinedAnalysisListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return false;

  CombinedAnalysisRoles analysisRole = static_cast<CombinedAnalysisRoles>(role);
  if (analysisRole != CombinedAnalysisRoles::AnalysisVisibleRole)
    return false;

  if (isViewshed(index.row()))
    return m_viewshedModel->setData(m_viewshedModel->index(viewshedIndex(index.row()), 0), value, ViewshedListModel::ViewshedRoles::ViewshedVisibleRole);
  else if (isLineOfSight(index.row()))
    return m_lineOfSightModel->setData(m_lineOfSightModel->index(lineOfSightIndex(index.row()), 0), value, AnalysisListModel::AnalysisRoles::AnalysisVisibleRole);

  return false;
}

/*!
  \internal
 */
QHash<int, QByteArray> CombinedAnalysisListModel::roleNames() const
{
  return m_roles;
}

/*!
  \internal
 */
void CombinedAnalysisListModel::handleUnderlyingDataChanged()
{
  beginResetModel();
  endResetModel();
}

/*!
  \internal
 */
void CombinedAnalysisListModel::connectAnalysisListModelSignals(QAbstractItemModel* analysisList)
{
  if (analysisList == nullptr)
    return;

  // react to changes in the supplied model
  connect(analysisList, &QAbstractItemModel::dataChanged, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &QAbstractItemModel::modelReset, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &QAbstractItemModel::rowsInserted, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &QAbstractItemModel::rowsRemoved, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
}

/*!
  \internal
 */
int CombinedAnalysisListModel::viewshedCount() const
{
  return m_viewshedModel == nullptr ? 0 : m_viewshedModel->rowCount();
}

/*!
  \internal
 */
int CombinedAnalysisListModel::lineOfSightCount() const
{
  return m_lineOfSightModel == nullptr ? 0 : m_lineOfSightModel->rowCount();
}

/*!
  \internal
 */
bool CombinedAnalysisListModel::isViewshed(int row) const
{
  // determine whether the supplied row is within the range of the viewshed model
  return row < viewshedCount() && m_viewshedModel;
}

/*!
  \internal
 */
bool CombinedAnalysisListModel::isLineOfSight(int row) const
{
  // determine whether the supplied row is within the range of the line of sight model
  return row < (lineOfSightCount() + viewshedCount()) && m_lineOfSightModel;
}

/*!
  \internal
 */
int CombinedAnalysisListModel::viewshedIndex(int row) const
{
  return row;
}

/*!
  \internal
 */
int CombinedAnalysisListModel::lineOfSightIndex(int row) const
{
  return row - viewshedCount();
}

} // Dsa
