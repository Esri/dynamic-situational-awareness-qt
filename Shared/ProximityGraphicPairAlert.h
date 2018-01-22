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

#ifndef PROXIMITYGRAPHICTPAIRALERT_H
#define PROXIMITYGRAPHICTPAIRALERT_H

#include "ProximityPairAlert.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class Graphic;
}
}

class ProximityGraphicPairAlert : public ProximityPairAlert
{
  Q_OBJECT

public:
  explicit ProximityGraphicPairAlert(Esri::ArcGISRuntime::Graphic* graphic1,
                            Esri::ArcGISRuntime::Graphic* graphic2,
                            double distance,
                            QObject* parent = nullptr);
  ~ProximityGraphicPairAlert();

  void highlight(bool on) override;

  QString element1Description() const override;
  QString element2Description() const override;

private:
  Esri::ArcGISRuntime::Graphic* m_graphic1;
  Esri::ArcGISRuntime::Graphic* m_graphic2;
  QString m_description1;
  QString m_description2;
};

#endif // PROXIMITYGRAPHICTPAIRALERT_H
