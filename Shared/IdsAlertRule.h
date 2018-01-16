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

#ifndef IDSALERTRULE_H
#define IDSALERTRULE_H

#include "AbstractAlertRule.h"
#include "AbstractAlert.h"

#include <QSet>
#include <QUuid>

class IdsAlertRule : public AbstractAlertRule
{
  Q_OBJECT

public:
  explicit IdsAlertRule(QObject* parent = nullptr);
  ~IdsAlertRule();

  bool matchesRule(AbstractAlert* alert) const override;

  void addId(const QUuid& id);

private:
  QSet<QUuid> m_ids;
};

#endif // IDSALERTRULE_H
