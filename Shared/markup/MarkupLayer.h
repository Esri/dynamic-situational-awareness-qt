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

#ifndef MARKUPLAYER_H
#define MARKUPLAYER_H

namespace Esri {
namespace ArcGISRuntime {
class FeatureCollection;
class GraphicsOverlay;
class SimpleLineSymbol;
class Feature;
}
}

#include "FeatureCollectionLayer.h"
#include <QHash>

class MarkupLayer : public Esri::ArcGISRuntime::FeatureCollectionLayer
{
  Q_OBJECT

public:
  ~MarkupLayer();

  // factory create methods
  static MarkupLayer* createFromJson(const QString& json, QObject* parent = nullptr);
  static MarkupLayer* createFromPath(const QString& path);
  static MarkupLayer* createFromGraphics(Esri::ArcGISRuntime::GraphicsOverlay* graphicsOverlay, const QString& authorName, QObject* parent = nullptr);

  static QStringList colors();

  void setPath(const QString& path);
  QString path() const;

  QString toJson() const;
  Esri::ArcGISRuntime::FeatureCollection* featureCollection();

private:
  MarkupLayer(const QString& json, Esri::ArcGISRuntime::FeatureCollection* featureCollection, QObject* parent = nullptr);

  QString m_path;
  QString m_json;
  QString m_username;
  Esri::ArcGISRuntime::FeatureCollection* m_featureCollection = nullptr;
  QHash<QUuid, QPair<Esri::ArcGISRuntime::Feature*, Esri::ArcGISRuntime::SimpleLineSymbol*>> m_featureHash;
};

#endif // MARKUPLAYER_H
