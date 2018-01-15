// Copyright 2016 ESRI
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

#include <QAbstractListModel>
#include <QList>
#include <QMap>

class AbstractAlert;

class AlertListModel : public QAbstractListModel
{
  Q_OBJECT

public:

  enum AlertListRoles
  {
    AlertId = Qt::UserRole + 1,
    Message = Qt::UserRole + 2,
    Status = Qt::UserRole + 3,
    Position = Qt::UserRole +4
  };

  static AlertListModel* instance();

  ~AlertListModel();

  bool addAlert(AbstractAlert* alert);

  AbstractAlert* alertAt(int rowIndex) const;

  // QAbstractItemModel interface
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  AlertListModel(QObject* parent = nullptr);

  QHash<int, QByteArray>  m_roles;
  QList<AbstractAlert*>   m_alerts;
};

#endif // ALERT_LISTMODEL_H
