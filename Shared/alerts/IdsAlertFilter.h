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

#ifndef IDSALERTFILTER_H
#define IDSALERTFILTER_H

// example app headers
#include "AlertFilter.h"

// Qt headers
#include <QSet>
#include <QUuid>

namespace Dsa {

class AlertConditionData;

class IdsAlertFilter : public AlertFilter
{
  Q_OBJECT

public:
  explicit IdsAlertFilter(QObject* parent = nullptr);
  ~IdsAlertFilter();

  bool passesFilter(AlertConditionData* conditionData) const override;

  void addId(const QUuid& id);
  void clearIds();

private:
  QSet<QUuid> m_ids;
};

} // Dsa

#endif // IDSALERTFILTER_H
