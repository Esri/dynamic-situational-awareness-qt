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

#ifndef GRAPHICALERTSOURCE_H
#define GRAPHICALERTSOURCE_H

// dsa app headers
#include "AlertSource.h"

namespace Esri::ArcGISRuntime {
class Graphic;
}

namespace Dsa {

class GraphicAlertSource : public AlertSource
{
  Q_OBJECT

public:
  explicit GraphicAlertSource(Esri::ArcGISRuntime::Graphic* graphic);
  ~GraphicAlertSource();

  Esri::ArcGISRuntime::Point location() const override;
  QVariant value(const QString& key) const override;

  void setSelected(bool selected) override;

private:
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;
};

} // Dsa

#endif // GRAPHICALERTSOURCE_H
