// Copyright 2018 ESRI
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

protected:
  void update360Mode(bool is360Mode) override;

private:
  Q_DISABLE_COPY(GeoElementViewshed360)
  GeoElementViewshed360() = delete;

  QPointer<Esri::ArcGISRuntime::GeoElement> m_geoElement;
  QString m_headingAttribute;
  QString m_pitchAttribute;
};

} // Dsa

#endif // GEOELEMENTVIEWSHED360_H
