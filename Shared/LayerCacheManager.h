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

#ifndef LAYERCACHEMANAGER_H
#define LAYERCACHEMANAGER_H

namespace Esri {
namespace ArcGISRuntime {
class Layer;
}
}

#include "AbstractTool.h"

class LayerCacheManager : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

public:
  explicit LayerCacheManager(QObject* parent = nullptr);
  ~LayerCacheManager();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

private slots:
  void onLayerChanged(Esri::ArcGISRuntime::Layer* layer);
  void onLayerListChanged();

private:
  QStringList m_layers;
};

#endif // LAYERCACHEMANAGER_H
