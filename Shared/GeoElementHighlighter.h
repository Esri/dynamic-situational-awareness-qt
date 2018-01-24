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

#ifndef GEOELEMENTHIGHLIGHTER_H
#define GEOELEMENTHIGHLIGHTER_H

#include <QObject>

namespace Esri
{
namespace ArcGISRuntime
{
  class GeoElement;
  class GraphicsOverlay;
  class SimpleMarkerSceneSymbol;
}
}

class QTimer;

class GeoElementHighlighter : public QObject
{
  Q_OBJECT

public:
  explicit GeoElementHighlighter(QObject* parent = nullptr);
  ~GeoElementHighlighter();

  void setGeoElement(Esri::ArcGISRuntime::GeoElement* geoElement);
  void startHighlight();
  void stopHighlight();

private slots:
  void onGeoViewChanged();

private:
  Esri::ArcGISRuntime::GraphicsOverlay* m_highlightOverlay = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSceneSymbol* m_highlightSymbol = nullptr;
  Esri::ArcGISRuntime::GeoElement* m_geoElement = nullptr;
  QTimer* m_highlightTimer = nullptr;
};

#endif // GEOELEMENTHIGHLIGHTER_H
