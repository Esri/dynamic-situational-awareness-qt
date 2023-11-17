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

#ifndef WITHINDISTANCEALERTCONDITION_H
#define WITHINDISTANCEALERTCONDITION_H

// Qt headers
#include <QObject>

// dsa headers
#include "AlertCondition.h"

namespace Dsa {

class WithinDistanceAlertCondition : public AlertCondition
{
  Q_OBJECT

public:

  WithinDistanceAlertCondition(AlertLevel level,
                               const QString& name,
                               double distance,
                               QObject* parent = nullptr);

  ~WithinDistanceAlertCondition();

  AlertConditionData* createData(AlertSource* source, AlertTarget* target) override;

  QString queryString() const override;
  QVariantMap queryComponents() const override;

  double distance() const;

  static double getDistanceFromQueryComponents(const QVariantMap& queryComponents);

private:
  double m_distance;
};

} // Dsa

#endif // WITHINDISTANCEALERTCONDITION_H
