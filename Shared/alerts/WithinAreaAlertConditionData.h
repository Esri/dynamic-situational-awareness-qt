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

#ifndef WITHINAREAALERTCONDITIONDATA_H
#define WITHINAREAALERTCONDITIONDATA_H

// dsa app headers
#include "AlertConditionData.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
class Graphic;
}
}

namespace Dsa {

class WithinAreaAlertConditionData : public AlertConditionData
{
  Q_OBJECT

public:
  WithinAreaAlertConditionData(const QString& name,
                               AlertLevel level,
                               AlertSource* source,
                               AlertTarget* target,
                               QObject* parent = nullptr);

  ~WithinAreaAlertConditionData();

  bool matchesQuery() const override;
};

} // Dsa

#endif // WITHINAREAALERTCONDITIONDATA_H
