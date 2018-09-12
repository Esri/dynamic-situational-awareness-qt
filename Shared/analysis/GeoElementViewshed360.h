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

#ifndef GEOELEMENTVIEWSHED360_H
#define GEOELEMENTVIEWSHED360_H

// example app headers
#include "Viewshed360.h"

namespace Esri {
  namespace ArcGISRuntime {
    class GeoElement;
  }
}

namespace Dsa {

class GeoElementSignaler;
class GeoElementViewshed360 : public Viewshed360
{
  Q_OBJECT

public:
  GeoElementViewshed360(Esri::ArcGISRuntime::GeoElement* geoElement, Esri::ArcGISRuntime::AnalysisOverlay* analysisOverlay,
                  const QString& headingAttribute, const QString& pitchAttribute, QObject* parent = nullptr);
  ~GeoElementViewshed360();

  Esri::ArcGISRuntime::GeoElement* geoElement() const;

  double heading() const override;
  void setHeading(double heading) override;

  double pitch() const override;
  void setPitch(double pitch) override;

  double offsetZ() const override;
  void setOffsetZ(double offsetZ) override;

  bool isHeadingEnabled() const override;
  bool isPitchEnabled() const override;
  bool isOffsetZEnabled() const override;

  QString headingAttribute() const;
  QString pitchAttribute() const;

private:
  Q_DISABLE_COPY(GeoElementViewshed360)
  GeoElementViewshed360() = delete;

  QPointer<GeoElementSignaler> m_geoElementSignaler;
  QString m_headingAttribute;
  QString m_pitchAttribute;
};

} // Dsa

#endif // GEOELEMENTVIEWSHED360_H
