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

#ifndef IDENTIFYFEATURESCONTROLLER_H
#define IDENTIFYFEATURESCONTROLLER_H

// Qt headers
#include <QMouseEvent>
#include <QObject>

// C++ API headers
#include "TaskWatcher.h"

// dsa headers
#include "AbstractTool.h"

namespace Esri::ArcGISRuntime {
  class GeoElement;
  class IdentifyGraphicsOverlayResult;
  class IdentifyLayerResult;
  class PopupManager;
}

Q_MOC_INCLUDE("IdentifyGraphicsOverlayResult.h")
Q_MOC_INCLUDE("IdentifyLayerResult.h")

namespace Dsa {

class IdentifyController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
  Q_PROPERTY(QVariantList popupManagers READ popupManagers NOTIFY popupManagersChanged)

public:

  explicit IdentifyController(QObject* parent = nullptr);
  ~IdentifyController();

  QString toolName() const override;

  void setActive(bool active) override;

  bool busy() const;
  QVariantList popupManagers() const;

  void showPopup(Esri::ArcGISRuntime::GeoElement* geoElement, const QString& popupTitle);
  void showPopups(const QHash<QString, QList<Esri::ArcGISRuntime::GeoElement*>>& geoElementsByTitle);

private slots:
  void onMouseClicked(QMouseEvent& event);
  void onIdentifyLayersCompleted(const QUuid& taskId, QList<Esri::ArcGISRuntime::IdentifyLayerResult*> identifyResults);
  void onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*> identifyResults);

signals:
  void busyChanged();
  void popupManagersChanged();

private:
  bool addGeoElementPopup(Esri::ArcGISRuntime::GeoElement* geoElement, const QString& popupTitle);

  double m_tolerance = 5.0;
  Esri::ArcGISRuntime::TaskWatcher m_layersWatcher;
  Esri::ArcGISRuntime::TaskWatcher m_graphicsOverlaysWatcher;
  QList<Esri::ArcGISRuntime::PopupManager*> m_popupManagers;
};

// some shortcuts for working with multiple identify operation futures in a single 'QtFuture::whenAll' handler
namespace IdentifyResultsVariant {
  using FutureType = std::variant<QFuture<QList<Esri::ArcGISRuntime::IdentifyLayerResult*>>, QFuture<QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*>>>;
  namespace Types {
    constexpr uchar LAYERS = 0;
    constexpr uchar GRAPHICS = 1;
  }
}

} // Dsa

#endif // IDENTIFYFEATURESCONTROLLER_H
