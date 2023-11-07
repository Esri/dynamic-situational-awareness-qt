/*******************************************************************************
 *  Copyright 2012-2018 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/

#ifndef POINTHIGHLIGHTER_H
#define POINTHIGHLIGHTER_H

// Qt headers
#include <QObject>

// C++ API headers
#include "Point.h"

namespace Esri::ArcGISRuntime {
  class GraphicsOverlay;
  class SimpleMarkerSceneSymbol;
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
