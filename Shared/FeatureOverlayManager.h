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

#ifndef FEATUREOVERLAYMANAGER_H
#define FEATUREOVERLAYMANAGER_H

#include "AbstractOverlayManager.h"

#include <QHash>

namespace Esri
{
namespace ArcGISRuntime
{
  class FeatureLayer;
}
}

class FeatureOverlayManager : public AbstractOverlayManager
{
  Q_OBJECT

public:
  explicit FeatureOverlayManager(Esri::ArcGISRuntime::FeatureLayer* overlay, QObject* parent = nullptr);
  ~FeatureOverlayManager();

  void setSelected(Esri::ArcGISRuntime::GeoElement* element, bool ons) override;
  QString elementDescription(Esri::ArcGISRuntime::GeoElement* element) const;

  Esri::ArcGISRuntime::GeoElement* elementAt(int elementId) const override;

  qint64 numberOfElements() const override;

private:
  QString m_oidFieldName;
  Esri::ArcGISRuntime::FeatureLayer* m_overlay;
  mutable QHash<int, Esri::ArcGISRuntime::GeoElement*> m_elementCache;
};

#endif // FEATUREOVERLAYMANAGER_H
