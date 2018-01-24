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

#ifndef PROXIMITYPAIRALERT_H
#define PROXIMITYPAIRALERT_H

#include "AbstractAlert.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
}
}

class AbstractOverlayManager;

class ProximityPairAlert : public AlertConditionData
{
  Q_OBJECT

public:
  explicit ProximityPairAlert(Esri::ArcGISRuntime::GeoElement* element1,
                              Esri::ArcGISRuntime::GeoElement* element2,
                              AbstractOverlayManager* overlay1Manager,
                              AbstractOverlayManager* overlay2Manager,
                              double distance,
                              QObject* parent = nullptr);
  ~ProximityPairAlert();

  void highlight(bool on) override;

  Esri::ArcGISRuntime::Geometry position() const override;
  Esri::ArcGISRuntime::Geometry position2() const;

  double distance() const;

  Esri::ArcGISRuntime::GeoElement* geoElement() const override;

  virtual QString element1Description() const;
  virtual QString element2Description() const;

private:
  Esri::ArcGISRuntime::GeoElement* m_element1 = nullptr;
  Esri::ArcGISRuntime::GeoElement* m_element2 = nullptr;
  AbstractOverlayManager* m_overlay1Manager = nullptr;
  AbstractOverlayManager* m_overlay2Manager = nullptr;
  double m_distance;
};

#endif // PROXIMITYPAIRALERT_H
