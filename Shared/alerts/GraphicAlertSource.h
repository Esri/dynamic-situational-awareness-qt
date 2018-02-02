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

#ifndef GRAPHICALERTSOURCE_H
#define GRAPHICALERTSOURCE_H

#include "AlertSource.h"

namespace Esri
{
namespace ArcGISRuntime
{
class Graphic;
}
}

class GraphicAlertSource : public AlertSource
{
  Q_OBJECT

public:
  explicit GraphicAlertSource(Esri::ArcGISRuntime::Graphic* graphic);
  ~GraphicAlertSource();

  Esri::ArcGISRuntime::Point location() const override;
  QVariant value(const QString& key) const override;

  void setSelected(bool selected) override;

private:
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;
};

#endif // GRAPHICALERTSOURCE_H
