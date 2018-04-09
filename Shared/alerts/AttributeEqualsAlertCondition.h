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

#ifndef ATTRIBUTEEQUALSALERTCONDITION_H
#define ATTRIBUTEEQUALSALERTCONDITION_H


// example app headers
#include "AlertCondition.h"

// Qt headers
#include <QObject>

namespace Dsa {
namespace Alerts {

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

} // Alerts
} // Dsa

#endif // ATTRIBUTEEQUALSALERTCONDITION_H
