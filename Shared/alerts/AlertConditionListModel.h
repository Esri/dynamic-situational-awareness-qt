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

#ifndef ALERTCONDITION_LISTMODEL_H
#define ALERTCONDITION_LISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QList>

namespace Dsa {

class AlertCondition;

class AlertConditionListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum AlertConditionListRoles
  {
    ConditionName = Qt::UserRole + 2,
    Level = Qt::UserRole + 3,
    Description = Qt::UserRole + 4,
    ConditionEnabled = Qt::UserRole +5
  };

  explicit AlertConditionListModel(QObject* parent = nullptr);

  ~AlertConditionListModel();

  bool addAlertCondition(AlertCondition* condition);

  AlertCondition* conditionAt(int rowIndex) const;

  void removeAt(int rowIndex);

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:

  QHash<int, QByteArray> m_roles;
  QList<AlertCondition*> m_conditions;
};

} // Dsa

#endif // ALERTCONDITION_LISTMODEL_H
