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

#include "AbstractAlertRule.h"
#include "AlertListModel.h"
#include "AlertListProxyModel.h"

AlertListProxyModel::AlertListProxyModel(QObject* parent):
  QSortFilterProxyModel(parent)
{
  AlertListModel* sourceModel = AlertListModel::instance();
  if (sourceModel)
  {
    setSourceModel(sourceModel);
    connect(sourceModel, &AlertListModel::dataChanged, this, [this]()
    {
      invalidateFilter();
    });
  }
}

AlertListProxyModel::~AlertListProxyModel()
{

}

void AlertListProxyModel::applyFilter(const QList<AbstractAlertRule*>& rules)
{
  m_rules = rules;
  invalidate();
}

bool AlertListProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex&) const
{
  AlertListModel* sourceModel = AlertListModel::instance();
  if (!sourceModel)
    return false;

  AbstractAlert* alert = sourceModel->alertAt(sourceRow);
  if (!alert)
    return false;

  auto rulesIt = m_rules.cbegin();
  auto rulesEnd = m_rules.cend();
  for (; rulesIt != rulesEnd; ++rulesIt)
  {
    const auto rule = *rulesIt;
    if (!rule)
      continue;

    if (!rule->matchesRule(alert))
      return false;
  }

  return true;
}
