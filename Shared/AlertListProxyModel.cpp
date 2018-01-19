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

#include "AbstractAlert.h"
#include "AbstractAlertRule.h"
#include "AlertListModel.h"
#include "AlertListProxyModel.h"

#include <QTimer>

AlertListProxyModel::AlertListProxyModel(QObject* parent):
  QSortFilterProxyModel(parent),
  m_updateTimer(new QTimer(this))
{
  AlertListModel* sourceModel = AlertListModel::instance();
  if (sourceModel)
  {
    setSourceModel(sourceModel);
    connect(sourceModel, &AlertListModel::dataChanged, this, [this](const QModelIndex& topLeft)
    {
      const bool inModel = m_currentSourceRows.contains(topLeft.row());
      const bool shouldBeInModel = passesAllRules(topLeft.row());

      if (inModel != shouldBeInModel)
        invalidate();
    });

    connect(sourceModel, &AlertListModel::rowsInserted, this, [this]()
    {
      invalidate();
    });
  }

  m_updateTimer->setInterval(500);
  m_updateTimer->setSingleShot(false);
  connect(m_updateTimer, &QTimer::timeout, this, [this]()
  {
    invalidate();
  });
  m_updateTimer->start();
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
  const bool res = passesAllRules(sourceRow);

  if (res)
    m_currentSourceRows.insert(sourceRow);
  else
    m_currentSourceRows.remove(sourceRow);

  return res;
}

bool AlertListProxyModel::passesAllRules(int sourceRow) const
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
