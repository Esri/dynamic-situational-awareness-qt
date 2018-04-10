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
