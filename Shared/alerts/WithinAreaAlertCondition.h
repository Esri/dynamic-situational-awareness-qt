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

#ifndef WITHINAREAALERTCONDITION_H
#define WITHINAREAALERTCONDITION_H

// dsa app headers
#include "AlertCondition.h"

// Qt headers
#include <QObject>

namespace Dsa {

class WithinAreaAlertCondition : public AlertCondition
{
  Q_OBJECT

public:
  WithinAreaAlertCondition(AlertLevel level,
                           const QString& name,
                           QObject* parent = nullptr);

  ~WithinAreaAlertCondition();

  AlertConditionData* createData(AlertSource* source, AlertTarget* target) override;

  QString queryString() const override;
  QVariantMap queryComponents() const override;

  static QString isWithinQueryString();
};

} // Dsa

#endif // WITHINAREAALERTCONDITION_H
