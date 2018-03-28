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
}
}

#include "FeatureCollectionLayer.h"
#include <QObject>

class MarkupLayer : public Esri::ArcGISRuntime::FeatureCollectionLayer
{
  Q_OBJECT

public:
  MarkupLayer(Esri::ArcGISRuntime::FeatureCollection* featureCollection, QObject* parent = nullptr);
  ~MarkupLayer();

  void setPath(const QString& path);
  QString path() const;

private:
  QString m_path;
};

#endif // MARKUPLAYER_H
