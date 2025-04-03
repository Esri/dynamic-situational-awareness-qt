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

#ifndef DRAWORDERLAYERLISTMODEL_H
#define DRAWORDERLAYERLISTMODEL_H

// Qt headers
#include <QSortFilterProxyModel>

namespace Dsa {

class DrawOrderLayerListModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  explicit DrawOrderLayerListModel(QObject* parent = nullptr);
  ~DrawOrderLayerListModel();

protected:
  bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
};

} // Dsa

#endif // DRAWORDERLAYERLISTMODEL_H
