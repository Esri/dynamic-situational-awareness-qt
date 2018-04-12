// Copyright 2018 ESRI
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

#ifndef POINTHIGHLIGHTER_H
#define POINTHIGHLIGHTER_H

// C++ API headers
#include "Point.h"

// Qt headers
#include <QObject>

namespace Esri {
namespace ArcGISRuntime {
class GraphicsOverlay;
class SimpleMarkerSceneSymbol;
}
}

class QTimer;

namespace Dsa {

class PointHighlighter : public QObject
{
  Q_OBJECT

public:
  explicit PointHighlighter(QObject* parent = nullptr);
  ~PointHighlighter();

  void startHighlight();
  void stopHighlight();

public slots:
  void onPointChanged(const Esri::ArcGISRuntime::Point& point);

private slots:
  void onGeoViewChanged();

private:
  Esri::ArcGISRuntime::GraphicsOverlay* m_highlightOverlay = nullptr;
  Esri::ArcGISRuntime::SimpleMarkerSceneSymbol* m_highlightSymbol = nullptr;
  Esri::ArcGISRuntime::Point m_point;
  QTimer* m_highlightTimer = nullptr;
  QMetaObject::Connection m_timerConnection;
};

} // Dsa

#endif // POINTHIGHLIGHTER_H
