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

#ifndef MESSAGESOVERLAY_H
#define MESSAGESOVERLAY_H

// Qt headers
#include <QHash>
#include <QObject>
#include <QPointer>

namespace Esri::ArcGISRuntime {
class GeoView;
class Renderer;
class DynamicEntityLayer;
enum class SurfacePlacement;
}

namespace Dsa {

class MessageFeed;

class MessagesOverlay : public QObject
{
  Q_OBJECT

public:
  explicit MessagesOverlay(Esri::ArcGISRuntime::GeoView* geoView, QObject* parent = nullptr);
  MessagesOverlay(Esri::ArcGISRuntime::GeoView* geoView,
                  Esri::ArcGISRuntime::Renderer* renderer,
                  const QString& messageType,
                  MessageFeed* messageFeed,
                  Esri::ArcGISRuntime::SurfacePlacement surfacePlacement,
                  QObject* parent = nullptr);
  ~MessagesOverlay();

  Esri::ArcGISRuntime::Renderer* renderer() const;
  void setRenderer(Esri::ArcGISRuntime::Renderer* renderer);

  Esri::ArcGISRuntime::SurfacePlacement surfacePlacement() const;
  void setSurfacePlacement(Esri::ArcGISRuntime::SurfacePlacement surfacePlacement);

  QString messageType() const;
  void setMessageType(const QString& messageType);

  Esri::ArcGISRuntime::DynamicEntityLayer* dynamicEntityLayer() const;

  Esri::ArcGISRuntime::GeoView* geoView() const;

  bool isVisible() const;
  void setVisible(bool visible);

signals:
  void visibleChanged();
  void errorOccurred(const QString& error);

private:
  Q_DISABLE_COPY(MessagesOverlay)

  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  QPointer<Esri::ArcGISRuntime::Renderer> m_renderer;
  Esri::ArcGISRuntime::SurfacePlacement m_surfacePlacement;

  Esri::ArcGISRuntime::DynamicEntityLayer* m_dynamicEntityLayer = nullptr;
  MessageFeed* m_messageFeed = nullptr;
};

} // Dsa

#endif // MESSAGESOVERLAY_H
