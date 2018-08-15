/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

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
