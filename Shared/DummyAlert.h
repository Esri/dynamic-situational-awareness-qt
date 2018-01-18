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

#ifndef DUMMYALERT_H
#define DUMMYALERT_H

#include "AbstractAlert.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class Graphic;
}
}

class DummyAlert : public AbstractAlert
{
  Q_OBJECT

public:
  explicit DummyAlert(Esri::ArcGISRuntime::Graphic* graphic, QObject* parent = nullptr);
  ~DummyAlert();

  Esri::ArcGISRuntime::Geometry position() const override;
  void highlight(bool on) override;

private:
  Esri::ArcGISRuntime::Graphic* m_graphic;
};

#endif // DUMMYALERT_H
