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

#ifndef ATTRIBUTEEQUALSALERTCONDITIONDATA_H
#define ATTRIBUTEEQUALSALERTCONDITIONDATA_H

#include "AlertConditionData.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
class Graphic;
}
}

class AttributeEqualsAlertConditionData : public AlertConditionData
{
  Q_OBJECT

public:

  AttributeEqualsAlertConditionData(const QString& name,
                                    AlertLevel level,
                                    AlertSource* source,
                                    AlertTarget* target,
                                    const QString& attributeName,
                                    QObject* parent = nullptr);

  ~AttributeEqualsAlertConditionData();

  bool matchesQuery() const override;

  QString attributeName() const;

private:
  QString m_attributeName;
};

#endif // ATTRIBUTEEQUALSALERTCONDITIONDATA_H
