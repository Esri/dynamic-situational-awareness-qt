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
#include <QSet>

class AbstractAlertRule;

class AlertListProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:

  AlertListProxyModel(QObject* parent = nullptr);
  ~AlertListProxyModel();

  void applyFilter(const QList<AbstractAlertRule*>& rules);

  // QSortFilterProxyModel interface
protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
  bool passesAllRules(int sourceRow) const;

  QList<AbstractAlertRule*> m_rules;
  mutable QSet<int> m_currentSourceRows;
};

#endif // ALERTLISTPROXYMODEL_H
