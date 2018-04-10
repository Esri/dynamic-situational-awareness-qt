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

#ifndef LAYERRESULTSMANAGER_H
#define LAYERRESULTSMANAGER_H

// Qt headers
#include <QList>

namespace Esri {
namespace ArcGISRuntime {
  class IdentifyLayerResult;
}
}

namespace Dsa {

struct LayerResultsManager {

  QList<Esri::ArcGISRuntime::IdentifyLayerResult*> m_results;

  explicit LayerResultsManager(const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& results);
  ~LayerResultsManager();
};

} // Dsa

#endif // LAYERRESULTSMANAGER_H
