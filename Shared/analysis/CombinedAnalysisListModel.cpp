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

#include "CombinedAnalysisListModel.h"

#include "AnalysisListModel.h"
#include "AnalysisOverlay.h"
#include "AnalysisOverlayListModel.h"
#include "SceneView.h"

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

void CombinedAnalysisListModel::setSceneView(SceneView* sceneView)
{
  beginResetModel();
  m_sceneView = sceneView;
  endResetModel();

  AnalysisOverlayListModel* overlays = m_sceneView->analysisOverlays();
  if (overlays == nullptr)
    return;

  connect(overlays, &AnalysisOverlayListModel::dataChanged, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(overlays, &AnalysisOverlayListModel::modelReset, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(overlays, &AnalysisOverlayListModel::rowsInserted, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(overlays, &AnalysisOverlayListModel::rowsRemoved, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);

  connect(overlays, &AnalysisOverlayListModel::analysisOverlayAdded, this, [this, overlays](int index)
  {
    AnalysisOverlay* overlay = overlays->at(index);
    if (overlay == nullptr)
      return;

    connectAnalysisListModelSignals(overlay->analyses());
  });

  const int analysisCount = overlays->rowCount();
  for (int i = 0; i < analysisCount; ++i)
  {
    AnalysisOverlay* overlay = overlays->at(i);
    if (overlay == nullptr)
      continue;

    connectAnalysisListModelSignals(overlay->analyses());
  }
}

Analysis* CombinedAnalysisListModel::analysisAt(int row) const
{
  if (m_sceneView == nullptr)
    return nullptr;

  AnalysisOverlayListModel* overlays = m_sceneView->analysisOverlays();
  if (overlays == nullptr)
    return nullptr;

  const int overlaysCount = overlays->rowCount();
  int currentRow = 0;
  for (int i = 0; i < overlaysCount; ++i)
  {
    AnalysisOverlay* overlay = overlays->at(i);
    if (overlay == nullptr)
      continue;

    AnalysisListModel* analysisList = overlay->analyses();
    if (analysisList == nullptr)
      continue;

    const int analysisCount = analysisList->rowCount();
    for (int j = 0; j < analysisCount; ++j)
    {
      Analysis* analysis = analysisList->at(j);
      if (!analysis)
        continue;

      if (currentRow == row)
        return analysis;

      currentRow++;
    }
  }

  return nullptr;
}

AnalysisOverlay* CombinedAnalysisListModel::overlayAt(int row) const
{
  if (m_sceneView == nullptr)
    return nullptr;

  AnalysisOverlayListModel* overlays = m_sceneView->analysisOverlays();
  if (overlays == nullptr)
    return nullptr;

  const int overlaysCount = overlays->rowCount();
  int currentRow = 0;
  for (int i = 0; i < overlaysCount; ++i)
  {
    AnalysisOverlay* overlay = overlays->at(i);
    if (overlay == nullptr)
      continue;

    AnalysisListModel* analysisList = overlay->analyses();
    if (analysisList == nullptr)
      continue;

    const int analysisCount = analysisList->rowCount();
    for (int j = 0; j < analysisCount; ++j)
    {
      Analysis* analysis = analysisList->at(j);
      if (!analysis)
        continue;

      if (currentRow == row)
        return overlay;

      currentRow++;
    }
  }

  return nullptr;
}

int CombinedAnalysisListModel::rowCount(const QModelIndex&) const
{
  if (m_sceneView == nullptr)
    return 0;

  AnalysisOverlayListModel* overlays = m_sceneView->analysisOverlays();
  if (overlays == nullptr)
    return 0;

  const int overlaysCount = overlays->rowCount();
  int totalAnalysis = 0;
  for (int i = 0; i < overlaysCount; ++i)
  {
    AnalysisOverlay* overlay = overlays->at(i);
    if (overlay == nullptr)
      continue;

    AnalysisListModel* analysisList = overlay->analyses();
    if (analysisList == nullptr)
      continue;

    const int analysisCount = analysisList->rowCount();
    for (int j = 0; j < analysisCount; ++j)
    {
      Analysis* analysis = analysisList->at(j);
      if (analysis)
        totalAnalysis++;
    }
  }

  return totalAnalysis;
}

QVariant CombinedAnalysisListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= rowCount(index))
    return QVariant();

  Analysis* analysis = analysisAt(index.row());
  if (analysis == nullptr)
    return QVariant();

  switch (role)
  {
  case AnalysisNameRole:
    return QString("Analysis %1").arg(QString::number(index.row()));
  case AnalysisVisibleRole:
    return analysis->isVisible();
  case AnalysisTypeRole:
    return QVariant::fromValue(analysis->analysisType());
  default:
    break;
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

  Analysis* analysis = analysisAt(index.row());
  if (analysis == nullptr)
    return false;

  analysis->setVisible(value.toBool());

  return true;
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

void CombinedAnalysisListModel::connectAnalysisListModelSignals(AnalysisListModel *analysisList)
{
  if (analysisList == nullptr)
    return;

  connect(analysisList, &AnalysisListModel::dataChanged, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &AnalysisListModel::modelReset, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &AnalysisListModel::rowsInserted, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
  connect(analysisList, &AnalysisListModel::rowsRemoved, this, &CombinedAnalysisListModel::handleUnderlyingDataChanged);
}
