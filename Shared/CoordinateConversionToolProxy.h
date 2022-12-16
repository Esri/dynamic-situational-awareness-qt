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

#ifndef COORDINATE_CONVERSION_TOOL_PROXY_H
#define COORDINATE_CONVERSION_TOOL_PROXY_H

// dsa headers
#include "AbstractTool.h"

namespace Esri::ArcGISRuntime::Toolkit {
class CoordinateConversionController;
class CoordinateConversionResult;
}

Q_MOC_INCLUDE("CoordinateConversionController.h")
Q_MOC_INCLUDE("CoordinateConversionResult.h")

namespace Dsa
{

class CoordinateConversionToolProxy : public AbstractTool
{
  Q_OBJECT
  Q_PROPERTY(Esri::ArcGISRuntime::Toolkit::CoordinateConversionController* controller READ controller CONSTANT)
  Q_PROPERTY(Esri::ArcGISRuntime::Toolkit::CoordinateConversionResult* inputFormat READ inputFormat CONSTANT)
  Q_PROPERTY(bool inInputMode READ inInputMode WRITE setInInputMode NOTIFY inInputModeChanged)
public:
  CoordinateConversionToolProxy(QObject* parent = nullptr);
  ~CoordinateConversionToolProxy() override;

  QString toolName() const override;

  bool handleClick(const Esri::ArcGISRuntime::Point& pos) override;

  void setProperties(const QVariantMap& properties) override;

  Esri::ArcGISRuntime::Toolkit::CoordinateConversionController* controller() const;

  Esri::ArcGISRuntime::Toolkit::CoordinateConversionResult* inputFormat() const;

  bool inInputMode() const;
  void setInInputMode(bool mode);
  Q_SIGNAL void inInputModeChanged();

private:
  void connectController();

  bool m_inInputMode;
  Esri::ArcGISRuntime::Toolkit::CoordinateConversionController* m_controller = nullptr;
  Esri::ArcGISRuntime::Toolkit::CoordinateConversionResult* m_inputFormat = nullptr;
};

} // Dsa

#endif // COORDINATE_CONVERSION_TOOL_PROXY_H
