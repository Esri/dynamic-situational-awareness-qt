#ifndef COMMON_H
#define COMMON_H

#include "DynamicEntity.h"
#include "GeoElement.h"
#include "IdentifyLayerResult.h"
#include "IdentifyGraphicsOverlayResult.h"

#include <QFuture>
#include <QList>
#include <QPointer>
#include <QString>

#include <tuple>
#include <variant>

namespace Dsa {
  namespace ContextMenu {
    using Element = std::tuple<QString, Esri::ArcGISRuntime::GeoElement*, QPointer<Esri::ArcGISRuntime::DynamicEntity>>;
  }
  namespace IdentifyResults {
    using Layer = QFuture<QList<Esri::ArcGISRuntime::IdentifyLayerResult*>>;
    using Graphics = QFuture<QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*>>;
    using Variant = std::variant<Layer, Graphics>;
  }
}
#endif // COMMON_H
