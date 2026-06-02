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
#include <QFuture>
#include <QMouseEvent>
#include <QObject>

// C++ API headers
#include "Popup.h"

// DSA headers
#include "AbstractTool.h"

// STL headers
#include <memory>
#include <vector>


namespace Esri::ArcGISRuntime {
  class GeoElement;
  class IdentifyGraphicsOverlayResult;
  class IdentifyLayerResult;
  class Popup;
}

namespace Dsa {

class IdentifyController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::Popup* popup READ popup NOTIFY popupChanged)
  Q_PROPERTY(bool canNext READ canNext NOTIFY popupChanged)
  Q_PROPERTY(bool canPrev READ canPrev NOTIFY popupChanged)

public:

  explicit IdentifyController(QObject* parent = nullptr);
  ~IdentifyController();

  QString toolName() const override;

  void showPopups(const QHash<QString, QList<Esri::ArcGISRuntime::GeoElement*>>& geoElementsByTitle);
  Q_INVOKABLE void nextPopup();
  Q_INVOKABLE void prevPopup();
  Q_INVOKABLE void clearPopups();

signals:
  void popupChanged();

private:
  bool addGeoElementPopup(Esri::ArcGISRuntime::GeoElement* geoElement, const QString& popupTitle);
  Esri::ArcGISRuntime::Popup* popup() const;
  bool canNext() const;
  bool canPrev() const;

  double m_tolerance = 5.0;
  int m_currentPopupIndex = -1;
  std::vector<Esri::ArcGISRuntime::Popup*> m_popups;
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
