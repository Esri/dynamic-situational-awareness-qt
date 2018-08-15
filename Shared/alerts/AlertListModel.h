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

#ifndef ALERT_LISTMODEL_H
#define ALERT_LISTMODEL_H

// Qt headers
#include <QAbstractListModel>
#include <QHash>
#include <QList>

namespace Dsa {

class AlertConditionData;

class AlertListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum AlertListRoles
  {
    AlertId = Qt::UserRole + 1,
    Name = Qt::UserRole + 2,
    Level = Qt::UserRole + 3,
    Viewed = Qt::UserRole + 4
  };

  static AlertListModel* instance();

  ~AlertListModel();

  bool addAlertConditionData(AlertConditionData* alert);
  void removeAlert(AlertConditionData* alert);

  AlertConditionData* alertAt(int rowIndex) const;

  void removeAt(int rowIndex);

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  AlertListModel(QObject* parent = nullptr);

  QHash<int, QByteArray>  m_roles;
  QList<AlertConditionData*>   m_alerts;
};

} // Dsa

#endif // ALERT_LISTMODEL_H
