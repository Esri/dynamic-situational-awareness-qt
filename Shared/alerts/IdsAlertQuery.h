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

#ifndef IDSALERTQUERY_H
#define IDSALERTQUERY_H

#include "AlertQuery.h"

#include <QSet>
#include <QUuid>

class AlertConditionData;

class IdsAlertQuery : public AlertQuery
{
  Q_OBJECT

public:
  explicit IdsAlertQuery(QObject* parent = nullptr);
  ~IdsAlertQuery();

  bool matchesRule(AlertConditionData* alert) const override;

  void addId(const QUuid& id);

private:
  QSet<QUuid> m_ids;
};

#endif // IDSALERTQUERY_H
