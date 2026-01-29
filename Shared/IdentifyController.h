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
#include "utilities/Common.h"
#include "AbstractTool.h"

namespace Esri::ArcGISRuntime {
  class DynamicEntity;
  class DynamicEntityChangedInfo;
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

  void setGeoElements(const std::vector<ContextMenu::Element>& contextElements);
  Q_INVOKABLE void nextPopup();
  Q_INVOKABLE void prevPopup();
  Q_INVOKABLE void reset();

signals:
  void popupChanged();

private:
  void setPopup();
  void resetGeoElements(const std::vector<ContextMenu::Element>& contextElements);
  Esri::ArcGISRuntime::Popup* popup() const;
  bool canNext() const;
  bool canPrev() const;
  void dynamicEntityChanged(Esri::ArcGISRuntime::DynamicEntityChangedInfo* deci);

  double m_tolerance = 5.0;
  int m_geoElementIndex = -1;
  Esri::ArcGISRuntime::Popup* m_popup = nullptr;
  std::vector<ContextMenu::Element> m_contextElements;
  QMetaObject::Connection m_dynamicEntityConnection;
};

} // Dsa

#endif // IDENTIFYFEATURESCONTROLLER_H
