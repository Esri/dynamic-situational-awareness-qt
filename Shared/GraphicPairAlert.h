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

#ifndef GRAPHICTPAIRALERT_H
#define GRAPHICTPAIRALERT_H

#include "AbstractAlert.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class Graphic;
}
}

class GraphicPairAlert : public AbstractAlert
{
  Q_OBJECT

public:
  explicit GraphicPairAlert(Esri::ArcGISRuntime::Graphic* graphic1,
                            Esri::ArcGISRuntime::Graphic* graphic2,
                            double distance,
                            QObject* parent = nullptr);
  ~GraphicPairAlert();

  Esri::ArcGISRuntime::Geometry position() const override;
  void highlight(bool on) override;

  Esri::ArcGISRuntime::Geometry position2() const;

  double distance() const;

private:
  Esri::ArcGISRuntime::Graphic* m_graphic1;
  Esri::ArcGISRuntime::Graphic* m_graphic2;
  double m_distance;
};

#endif // GRAPHICTPAIRALERT_H
