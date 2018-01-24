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

#ifndef GRAPHICSOVERLAYMANAGER_H
#define GRAPHICSOVERLAYMANAGER_H

#include "AbstractOverlayManager.h"

namespace Esri
{
namespace ArcGISRuntime
{
  class GraphicsOverlay;
}
}

class GraphicsOverlayManager : public AbstractOverlayManager
{
  Q_OBJECT

public:
  explicit GraphicsOverlayManager(Esri::ArcGISRuntime::GraphicsOverlay* overlay, QObject* parent = nullptr);
  ~GraphicsOverlayManager();

  void setSelected(Esri::ArcGISRuntime::GeoElement* element, bool ons) override;
  QString elementDescription(Esri::ArcGISRuntime::GeoElement* element) const;

  Esri::ArcGISRuntime::GeoElement* elementAt(int elementId) const override;

  qint64 numberOfElements() const override;

private:
  Esri::ArcGISRuntime::GraphicsOverlay* m_overlay;
};

#endif // GRAPHICSOVERLAYMANAGER_H
