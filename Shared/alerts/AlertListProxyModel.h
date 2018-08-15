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

#ifndef ALERTLISTPROXYMODEL_H
#define ALERTLISTPROXYMODEL_H

// Qt headers
#include <QHash>
#include <QList>
#include <QSortFilterProxyModel>

namespace Dsa {

class AlertFilter;
class AlertListModel;

class AlertListProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:

  AlertListProxyModel(AlertListModel* sourceModel, QObject* parent = nullptr);
  ~AlertListProxyModel();

  void applyFilter(const QList<AlertFilter*>& filters);

  // QSortFilterProxyModel interface
protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
  bool passesAllQueries(int sourceRow) const;

  AlertListModel* m_sourceModel;
  QList<AlertFilter*> m_filters;
  mutable QHash<int, bool> m_rowsInModel;
};

} // Dsa

#endif // ALERTLISTPROXYMODEL_H
