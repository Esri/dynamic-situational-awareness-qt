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

#include "CombinedAnalysisListModel.h"
#include "ViewshedListModel.h"
#include "Viewshed360.h"

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

/*!
  \brief Constructor taking an optional \a parent.
 */
CombinedAnalysisListModel::CombinedAnalysisListModel(QObject* parent):
  QAbstractListModel(parent)
{
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
  beginResetModel();
  m_lineOfSightModel = lineOfSightModel;
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

  if (isViewshed(index.row()))
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
  else if (isLineOfSight(index.row()))
  {
    switch (role)
    {
    case AnalysisNameRole:
      return QString("Line of Sight %1").arg(QString::number(lineOfSightIndex(index.row())));
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
  return row < viewshedCount() && m_viewshedModel;
}

/*!
  \internal
 */
bool CombinedAnalysisListModel::isLineOfSight(int row) const
{
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
