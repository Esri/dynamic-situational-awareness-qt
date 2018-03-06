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

#ifndef COMBINEDANALYSISLISTMODEL_H
#define COMBINEDANALYSISLISTMODEL_H

#include <QAbstractListModel>

namespace Esri
{
namespace ArcGISRuntime
{
  class Analysis;
  class AnalysisListModel;
  class AnalysisOverlay;
  class SceneView;
}
}

class CombinedAnalysisListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum CombinedAnalysisRoles
  {
    AnalysisNameRole = Qt::UserRole + 1,
    AnalysisVisibleRole,
    AnalysisTypeRole
  };

  explicit CombinedAnalysisListModel(QObject* parent = nullptr);
  ~CombinedAnalysisListModel();

  void setSceneView(Esri::ArcGISRuntime::SceneView* sceneView);

  Esri::ArcGISRuntime::Analysis* analysisAt(int row) const;
  Esri::ArcGISRuntime::AnalysisOverlay* overlayAt(int row) const;

  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private slots:
  void handleUnderlyingDataChanged();

private:
  void connectAnalysisListModelSignals(Esri::ArcGISRuntime::AnalysisListModel* analysisList);

  QHash<int, QByteArray>                  m_roles;
  Esri::ArcGISRuntime::SceneView* m_sceneView = nullptr;
};

#endif // COMBINEDANALYSISLISTMODEL_H
