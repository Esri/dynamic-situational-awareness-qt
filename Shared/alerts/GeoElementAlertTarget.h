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

#ifndef GEOELEMENTALERTTARGET_H
#define GEOELEMENTALERTTARGET_H

namespace Esri::ArcGISRuntime {
  class GeoElement;
}

namespace Dsa {

class AlertTarget;
class GeoElementSignaler;
class GeoElementAlertTarget : public AlertTarget
{
  Q_OBJECT

public:
  explicit GeoElementAlertTarget(Esri::ArcGISRuntime::GeoElement* geoElement);
  ~GeoElementAlertTarget();

  QList<Esri::ArcGISRuntime::Geometry> targetGeometries(const Esri::ArcGISRuntime::Envelope& targetArea) const override;
  QVariant targetValue() const override;

private:
  GeoElementSignaler* m_geoElementSignaler = nullptr;
};

} // Dsa

#endif // GEOELEMENTALERTTARGET_H
