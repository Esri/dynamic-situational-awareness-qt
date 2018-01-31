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

#include "AlertConditionData.h"
#include "AlertQuery.h"
#include "AlertListModel.h"
#include "AlertListProxyModel.h"

AlertListProxyModel::AlertListProxyModel(QObject* parent):
  QSortFilterProxyModel(parent)
{
  AlertListModel* sourceModel = AlertListModel::instance();
  if (sourceModel)
  {
    setSourceModel(sourceModel);
    connect(sourceModel, &AlertListModel::dataChanged, this, [this](const QModelIndex& topLeft)
    {
      const bool inModel = m_rowsInModel.value(topLeft.row(), false);
      const bool shouldBeInModel = passesAllQueries(topLeft.row());

      if (inModel != shouldBeInModel)
      {
        m_rowsInModel.insert(topLeft.row(), shouldBeInModel);
        invalidate();
      }
    });

    connect(sourceModel, &AlertListModel::rowsInserted, this, [this](const QModelIndex&, int first, int)
    {
      // only clear the cache if this is an insert rather than an append
      if (m_rowsInModel.contains(first))
        m_rowsInModel.clear();

      invalidate();
    });

    connect(sourceModel, &AlertListModel::rowsRemoved, this, [this]()
    {
      m_rowsInModel.clear();
      invalidate();
    });
  }
}

AlertListProxyModel::~AlertListProxyModel()
{

}

void AlertListProxyModel::applyFilter(const QList<AlertQuery*>& rules)
{
  m_queries = rules;
  m_rowsInModel.clear();
  invalidate();
}

bool AlertListProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex&) const
{
  if (!m_rowsInModel.contains(sourceRow))
    m_rowsInModel.insert(sourceRow, passesAllQueries(sourceRow));

  return m_rowsInModel.value(sourceRow);
}

bool AlertListProxyModel::passesAllQueries(int sourceRow) const
{
  AlertListModel* sourceModel = AlertListModel::instance();
  if (!sourceModel)
    return false;

  AlertConditionData* alert = sourceModel->alertAt(sourceRow);
  if (!alert)
    return false;

  bool shouldBeActive = true;

  auto rulesIt = m_queries.cbegin();
  auto rulesEnd = m_queries.cend();
  for (; rulesIt != rulesEnd; ++rulesIt)
  {
    const auto rule = *rulesIt;
    if (!rule)
      continue;

    if (!rule->matchesRule(alert))
    {
      shouldBeActive = false;
      break;
    }
  }

  alert->setActive(shouldBeActive);

  return shouldBeActive;
}
