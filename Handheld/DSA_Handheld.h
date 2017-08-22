// Copyright 2016 ESRI
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

#ifndef DSA_HANDHELD_H
#define DSA_HANDHELD_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
}
}

#include <QQuickItem>

class DSA_Handheld : public QQuickItem
{
  Q_OBJECT

public:
  DSA_Handheld(QQuickItem* parent = nullptr);
  ~DSA_Handheld();

  void componentComplete() override;

private:
  Esri::ArcGISRuntime::Map*             m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView*    m_mapView = nullptr;

  QString m_dataPath;
};

#endif // DSA_HANDHELD_H
