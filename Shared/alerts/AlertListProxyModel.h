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

#ifndef ALERTLISTPROXYMODEL_H
#define ALERTLISTPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QList>
#include <QHash>

class AlertQuery;

class AlertListProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:

  AlertListProxyModel(QObject* parent = nullptr);
  ~AlertListProxyModel();

  void applyFilter(const QList<AlertQuery*>& rules);

  // QSortFilterProxyModel interface
protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
  bool passesAllQueries(int sourceRow) const;

  QList<AlertQuery*> m_queries;
  mutable QHash<int, bool> m_rowsInModel;
};

#endif // ALERTLISTPROXYMODEL_H
