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

#ifndef ABSTRACTOVERLAYMANAGER_H
#define ABSTRACTOVERLAYMANAGER_H

#include "QObject.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class GeoElement;
}
}

class AbstractOverlayManager : public QObject
{
  Q_OBJECT

public:
  explicit AbstractOverlayManager(QObject* parent = nullptr);
  ~AbstractOverlayManager();

  virtual void setSelected(Esri::ArcGISRuntime::GeoElement* element, bool on) = 0;
  virtual QString elementDescription(Esri::ArcGISRuntime::GeoElement* element) const = 0;
};

#endif // ABSTRACTOVERLAYMANAGER_H
