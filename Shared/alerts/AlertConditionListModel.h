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

#ifndef ALERTCONDITION_LISTMODEL_H
#define ALERTCONDITION_LISTMODEL_H

#include <QAbstractListModel>
#include <QList>

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

#endif // ALERTCONDITION_LISTMODEL_H
