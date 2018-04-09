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

// PCH header
#include "pch.hpp"

#include "AlertListProxyModel.h"

// example app headers
#include "AlertConditionData.h"
#include "AlertFilter.h"
#include "AlertListModel.h"

namespace Dsa
{
namespace Alerts
{

/*!
  \class AlertListProxyModel
  \inherits QSortFilterProxyModel
  \brief A proxy model responsible for filtering the list of \l AlertConditionData
  to show only those which are active and statisfy the current set of \l AlertFilter tests.
  */

/*!
  \brief Constructor for a new proxy model taking a \a sourceModel and an optional \a parent.
 */
AlertListProxyModel::AlertListProxyModel(AlertListModel* sourceModel, QObject* parent):
  QSortFilterProxyModel(parent),
  m_sourceModel(sourceModel)
{
  setSourceModel(m_sourceModel);

  // handle changes to a given condition data in the underlying AlertListModel
  connect(m_sourceModel, &AlertListModel::dataChanged, this, [this](const QModelIndex& topLeft)
  {
    // check whether the changed condition data is currently included in the filtered model
    const bool inModel = m_rowsInModel.value(topLeft.row(), false);

    // check whether the changed condition should now be included
    const bool shouldBeInModel = passesAllQueries(topLeft.row());

    // if the condition data has changed state, cache the new state and re-run the filter
    if (inModel != shouldBeInModel)
    {
      m_rowsInModel.insert(topLeft.row(), shouldBeInModel);
      invalidate();
    }
  });

  // handle the addition of a new condition data in the underlying AlertListModel
  connect(m_sourceModel, &AlertListModel::rowsInserted, this, [this](const QModelIndex&, int first, int)
  {
    // only clear the cache if this is an insert rather than an append
    if (m_rowsInModel.contains(first))
      m_rowsInModel.clear();

    // re-run the filter
    invalidate();
  });

  // handle condition data being removed from the underlying AlertListModel
  connect(m_sourceModel, &AlertListModel::rowsRemoved, this, [this]()
  {
    m_rowsInModel.clear();
    invalidate();
  });

}

/*!
  \brief Destructor.
 */
AlertListProxyModel::~AlertListProxyModel()
{

}

/*!
  \brief Applies a new set of \a filters to the condition data in the underlying \l AlertListModel.
 */
void AlertListProxyModel::applyFilter(const QList<AlertFilter*>& filters)
{
  m_filters = filters;
  m_rowsInModel.clear();
  invalidate();
}


/*!
  \brief Returns \c true if the condition data in the row indicated by \a sourceRow
  should be included in the model; otherwise returns \c false.
 */
bool AlertListProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex&) const
{
  // if required, update the cache to record the filter state for the condition data
  if (!m_rowsInModel.contains(sourceRow))
    m_rowsInModel.insert(sourceRow, passesAllQueries(sourceRow));

  return m_rowsInModel.value(sourceRow);
}

/*!
  \brief Internal.

  Apply all filters to the condition data at \a row
 */
bool AlertListProxyModel::passesAllQueries(int sourceRow) const
{
  // if the condition data is invalid, stop
  AlertConditionData* conditionData = m_sourceModel->alertAt(sourceRow);
  if (!conditionData)
    return false;

  // skip conditions data which is disabled
  if (!conditionData->isConditionEnabled())
    return false;

  // check whether and of the active AlertFilters exclude this condition data
  auto filterIt = m_filters.cbegin();
  auto filterEnd = m_filters.cend();
  for (; filterIt != filterEnd; ++filterIt)
  {
    const auto rule = *filterIt;
    if (!rule)
      continue;

    // if a rule excludes the condition data, it should not be in the model
    if (!rule->passesFilter(conditionData))
      return false;
  }

  // if the condition data passes all filters, it should be in the filtered model
  // if it currently satisfies its underlying condition
  return conditionData->matchesQuery();
}

} // Alerts
} // Dsa
