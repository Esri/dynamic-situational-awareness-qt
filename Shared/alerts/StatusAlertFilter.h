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

#ifndef STATUSALERTFILTER_H
#define STATUSALERTFILTER_H

// example app headers
#include "AlertConditionData.h"
#include "AlertFilter.h"

namespace Dsa {

class AlertConditionData;

class StatusAlertFilter : public AlertFilter
{
  Q_OBJECT

public:
  explicit StatusAlertFilter(QObject* parent = nullptr);
  ~StatusAlertFilter();

  bool passesFilter(AlertConditionData* alert) const override;

  AlertLevel minLevel() const;
  void setMinLevel(AlertLevel minLevel);

private:
  AlertLevel m_minLevel = AlertLevel::Low;
};

} // Dsa

#endif // STATUSALERTFILTER_H
