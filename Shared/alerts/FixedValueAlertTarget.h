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

#ifndef FIXEDVALUEALERTTARGET_H
#define FIXEDVALUEALERTTARGET_H

// Qt headers
#include <QVariant>

// dsa headers
#include "AlertTarget.h"

namespace Dsa {

class FixedValueAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit FixedValueAlertTarget(const QVariant& value, QObject* parent = nullptr);
  ~FixedValueAlertTarget();

  virtual QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;
  QVariant targetValue() const override;

private:
  QVariant m_value;
};

} // Dsa

#endif // FIXEDVALUEALERTTARGET_H
