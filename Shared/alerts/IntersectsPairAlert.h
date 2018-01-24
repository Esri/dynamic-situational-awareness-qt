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

#ifndef INTERSECTSPAIRALERT_H
#define INTERSECTSPAIRALERT_H

#include "AlertConditionData.h"

namespace Esri
{
namespace ArcGISRuntime
{
class GeoElement;
class Graphic;
}
}

class AbstractOverlayManager;

class IntersectsPairAlert : public AlertConditionData
{
  Q_OBJECT

public:
  explicit IntersectsPairAlert(Esri::ArcGISRuntime::Graphic* source,
                              Esri::ArcGISRuntime::GeoElement* target,
                              AlertCondition* parent = nullptr);
  ~IntersectsPairAlert();

  void highlight(bool on) override;

  Esri::ArcGISRuntime::Geometry position() const override;
  Esri::ArcGISRuntime::Geometry position2() const;

  Esri::ArcGISRuntime::GeoElement* geoElement() const override;

private:
  Esri::ArcGISRuntime::Graphic* m_source;
  Esri::ArcGISRuntime::GeoElement* m_target;
};

#endif // INTERSECTSPAIRALERT_H
