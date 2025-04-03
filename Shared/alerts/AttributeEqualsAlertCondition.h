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

#ifndef ATTRIBUTEEQUALSALERTCONDITION_H
#define ATTRIBUTEEQUALSALERTCONDITION_H

// Qt headers
#include <QObject>

// dsa Headers
#include "AlertCondition.h"

namespace Dsa {

class AttributeEqualsAlertCondition : public AlertCondition
{
  Q_OBJECT

public:
  AttributeEqualsAlertCondition(AlertLevel level,
                                const QString& name,
                                const QString& attributeName,
                                QObject* parent = nullptr);

  ~AttributeEqualsAlertCondition();

  AlertConditionData* createData(AlertSource* source, AlertTarget* target) override;

  QString queryString() const override;
  QVariantMap queryComponents() const override;

  static QString attributeNameFromQueryComponents(const QVariantMap& queryMap);

private:
  QString m_attributeName;
};

} // Dsa

#endif // ATTRIBUTEEQUALSALERTCONDITION_H
