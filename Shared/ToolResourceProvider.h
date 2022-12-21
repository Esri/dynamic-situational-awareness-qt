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

#ifndef TOOL_RESOURCE_PROVIDER_H
#define TOOL_RESOURCE_PROVIDER_H

#include <QObject>

#include "Point.h"
#include <QMouseEvent>
#include <QUuid>
#include <QCursor>

namespace Esri::ArcGISRuntime {
  class Basemap;
  class GeoView;
  class IdentifyGraphicsOverlayResult;
  class IdentifyLayerResult;
  class Map;
  class LayerListModel;
  class Scene;
  class SpatialReference;
}

Q_MOC_INCLUDE("IdentifyGraphicsOverlayResult.h")
Q_MOC_INCLUDE("IdentifyLayerResult.h")

namespace Dsa
{

class ToolResourceProvider : public QObject
{
  Q_OBJECT

public:

  static ToolResourceProvider* instance();

  ~ToolResourceProvider() override;

  Esri::ArcGISRuntime::Map* map() const;
  void setMap(Esri::ArcGISRuntime::Map* newMap);

  Esri::ArcGISRuntime::Scene* scene() const;
  void setScene(Esri::ArcGISRuntime::Scene* newScene);

  Esri::ArcGISRuntime::GeoView* geoView() const;
  void setGeoView(Esri::ArcGISRuntime::GeoView* newGeoView);

  Esri::ArcGISRuntime::SpatialReference spatialReference() const;

  Esri::ArcGISRuntime::LayerListModel* operationalLayers() const;

  void setBasemap(Esri::ArcGISRuntime::Basemap* newBasemap);

  void setMouseCursor(const QCursor& cursor);

  void clear();

public slots:
  void onMouseClicked(QMouseEvent& mouseEvent);
  void onMousePressed(QMouseEvent& mouseEvent);
  void onMouseMoved(QMouseEvent& mouseEvent);
  void onMouseReleased(QMouseEvent& mouseEvent);
  void onMousePressedAndHeld(QMouseEvent& mouseEvent);
  void onMouseDoubleClicked(QMouseEvent& mouseEvent);
  void onIdentifyGraphicsOverlayCompleted(QUuid taskId, Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult* identifyResult);
  void onIdentifyGraphicsOverlaysCompleted(QUuid taskId, const QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*>& identifyResults);
  void onIdentifyLayerCompleted(QUuid taskId, Esri::ArcGISRuntime::IdentifyLayerResult* identifyResult);
  void onIdentifyLayersCompleted(QUuid taskId, const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& identifyResults);
  void onScreenToLocationCompleted(QUuid taskId, const Esri::ArcGISRuntime::Point& location);
  void onLocationChanged(const Esri::ArcGISRuntime::Point& location);

signals:
  void sceneChanged();
  void geoViewChanged();
  void mapChanged();
  void spatialReferenceChanged();
  void identifyGraphicsOverlayCompleted(QUuid taskId, Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult* identifyResult);
  void identifyGraphicsOverlaysCompleted(QUuid taskId, const QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*>& identifyResults);
  void identifyLayerCompleted(QUuid taskId, Esri::ArcGISRuntime::IdentifyLayerResult* identifyResult);
  void identifyLayersCompleted(QUuid taskId, const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& identifyResults);
  void setMouseCursorRequested(const QCursor& cursor);
  void screenToLocationCompleted(QUuid taskId, const Esri::ArcGISRuntime::Point& location);
  void locationChanged(const Esri::ArcGISRuntime::Point& location);

  // mouse signals
  void mouseClicked(QMouseEvent& mouseEvent);
  void mouseClickedPoint(const Esri::ArcGISRuntime::Point& point);
  void mousePressed(QMouseEvent& mouseEvent);
  void mousePressedPoint(const Esri::ArcGISRuntime::Point& point);
  void mouseMoved(QMouseEvent& mouseEvent);
  void mouseMovedPoint(const Esri::ArcGISRuntime::Point& point);
  void mouseReleased(QMouseEvent& mouseEvent);
  void mouseReleasedPoint(const Esri::ArcGISRuntime::Point& point);
  void mousePressedAndHeld(QMouseEvent& mouseEvent);
  void mousePressedAndHeldPoint(const Esri::ArcGISRuntime::Point& point);
  void mouseDoubleClicked(QMouseEvent& mouseEvent);
  void mouseDoubleClickedPoint(const Esri::ArcGISRuntime::Point& point);

private:
  explicit ToolResourceProvider(QObject* parent = nullptr);

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::Scene* m_scene = nullptr;
};

} // Dsa

#endif // TOOL_RESOURCE_PROVIDER_H
