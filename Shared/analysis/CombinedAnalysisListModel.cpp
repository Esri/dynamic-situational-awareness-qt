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

CombinedAnalysisListModel::CombinedAnalysisListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[AnalysisNameRole] = "analysisName";
  m_roles[AnalysisVisibleRole] = "analysisVisible";
  m_roles[AnalysisTypeRole] = "analysisType";
}

CombinedAnalysisListModel::~CombinedAnalysisListModel()
{
}

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

void CombinedAnalysisListModel::setLineOfSightModel(AnalysisListModel* lineOfSightModel)
{
  beginResetModel();
  m_lineOfSightModel = lineOfSightModel;
  connectAnalysisListModelSignals(m_lineOfSightModel);
  endResetModel();
}

void CombinedAnalysisListModel::removeAt(int index)
{
  const int viewshedMaxIndex = viewshedCount();
  const int lineOfSightMaxIndex = viewshedMaxIndex + lineOfSightCount();

  if (index < viewshedMaxIndex && m_viewshedModel)
    m_viewshedModel->removeAt(index);
  else if (index < lineOfSightMaxIndex && m_lineOfSightModel)
    m_lineOfSightModel->removeAt(index - viewshedMaxIndex);
}

Point CombinedAnalysisListModel::locationAt(int index)
{
  const int viewshedMaxIndex = viewshedCount();
  const int lineOfSightMaxIndex = viewshedMaxIndex + lineOfSightCount();

  Analysis* analysis = nullptr;
  if (index < viewshedMaxIndex && m_viewshedModel)
  {
    Viewshed360* viewshed360 =  m_viewshedModel->at(index);
    if (viewshed360 != nullptr)
      analysis = viewshed360->viewshed();
  }
  else if (index < lineOfSightMaxIndex && m_lineOfSightModel)
  {
    analysis = m_lineOfSightModel->at(index - viewshedMaxIndex);
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

int CombinedAnalysisListModel::rowCount(const QModelIndex&) const
{
  return viewshedCount() + lineOfSightCount();
}

QVariant CombinedAnalysisListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return QVariant();

  const int viewshedMaxIndex = viewshedCount();
  const int lineOfSightMaxIndex = viewshedMaxIndex + lineOfSightCount();

  if (index.row() < viewshedMaxIndex && m_viewshedModel)
  {
    switch (role)
    {
    case AnalysisNameRole:
      return m_viewshedModel->data(m_viewshedModel->index(index.row(), 0), ViewshedListModel::ViewshedRoles::ViewshedNameRole);
    case AnalysisVisibleRole:
      return m_viewshedModel->data(m_viewshedModel->index(index.row(), 0), ViewshedListModel::ViewshedRoles::ViewshedVisibleRole);
    case AnalysisTypeRole:
      return QStringLiteral("viewshed");
    default:
      break;
    }
  }
  else if (index.row() < lineOfSightMaxIndex && m_lineOfSightModel)
  {
    switch (role)
    {
    case AnalysisNameRole:
      return QString("Line of Sight %1").arg(QString::number(index.row() - viewshedMaxIndex));
    case AnalysisVisibleRole:
      return m_lineOfSightModel->data(m_lineOfSightModel->index(index.row() - viewshedMaxIndex, 0), AnalysisListModel::AnalysisRoles::AnalysisVisibleRole);
    case AnalysisTypeRole:
      return QStringLiteral("lineOfSight");
    default:
      break;
    }
  }

  return QVariant();
}

bool CombinedAnalysisListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return false;

  CombinedAnalysisRoles analysisRole = static_cast<CombinedAnalysisRoles>(role);
  if (analysisRole != CombinedAnalysisRoles::AnalysisVisibleRole)
    return false;

  const int viewshedMaxIndex = viewshedCount();
  const int lineOfSightMaxIndex = viewshedMaxIndex + lineOfSightCount();

  if (index.row() < viewshedMaxIndex && m_viewshedModel)
   return m_viewshedModel->setData(m_viewshedModel->index(index.row(), 0), value, ViewshedListModel::ViewshedRoles::ViewshedVisibleRole);
  else if (index.row() < lineOfSightMaxIndex && m_lineOfSightModel)
      return m_lineOfSightModel->setData(m_lineOfSightModel->index(index.row() - viewshedMaxIndex, 0), value, AnalysisListModel::AnalysisRoles::AnalysisVisibleRole);

  return false;
}

QHash<int, QByteArray> CombinedAnalysisListModel::roleNames() const
{
  return m_roles;
}

void CombinedAnalysisListModel::handleUnderlyingDataChanged()
{
  beginResetModel();
  endResetModel();
}

void CombinedAnalysisListModel::connectAnalysisListModelSignals(QAbstractItemModel* analysisList)
{
  if (analysisList == nullptr)
    return;

  connect(analysisList, &QAbstractItemModel::dataChanged, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &QAbstractItemModel::modelReset, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &QAbstractItemModel::rowsInserted, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &QAbstractItemModel::rowsRemoved, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
}

int CombinedAnalysisListModel::viewshedCount() const
{
  return m_viewshedModel == nullptr ? 0 : m_viewshedModel->rowCount();
}

int CombinedAnalysisListModel::lineOfSightCount() const
{
  return m_lineOfSightModel == nullptr ? 0 : m_lineOfSightModel->rowCount();
}
