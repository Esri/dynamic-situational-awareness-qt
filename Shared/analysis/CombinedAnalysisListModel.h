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

#ifndef COMBINEDANALYSISLISTMODEL_H
#define COMBINEDANALYSISLISTMODEL_H

// Qt headers
#include <QAbstractListModel>

namespace Esri::ArcGISRuntime {
  class Analysis;
  class AnalysisListModel;
  class AnalysisOverlay;
  class Point;
}

namespace Dsa {

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
  QHash<Esri::ArcGISRuntime::Analysis*, int> m_lineOfSightIndices;
};

} // Dsa

#endif // COMBINEDANALYSISLISTMODEL_H
