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

#ifndef COMBINEDANALYSISLISTMODEL_H
#define COMBINEDANALYSISLISTMODEL_H


// qt_cpp headers
#include "Point.h"

// Qt headers
#include <QAbstractListModel>

namespace Esri
{
namespace ArcGISRuntime
{
  class Analysis;
  class AnalysisListModel;
  class AnalysisOverlay;
}
}

class ViewshedListModel;

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

  void setViewshedModel(QAbstractItemModel* viewshedModel);
  void setLineOfSightModel(Esri::ArcGISRuntime::AnalysisListModel* lineOfSightModel);

  void removeAt(int index);
  Esri::ArcGISRuntime::Point locationAt(int index);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private slots:
  void handleUnderlyingDataChanged();

private:
  void connectAnalysisListModelSignals(QAbstractItemModel* analysisList);
  int viewshedCount() const;
  int lineOfSightCount() const;
  bool isViewshed(int row) const;
  bool isLineOfSight(int row) const;
  int viewshedIndex(int row) const;
  int lineOfSightIndex(int row) const;

  QHash<int, QByteArray> m_roles;
  ViewshedListModel* m_viewshedModel = nullptr;
  Esri::ArcGISRuntime::AnalysisListModel* m_lineOfSightModel = nullptr;
};

#endif // COMBINEDANALYSISLISTMODEL_H
