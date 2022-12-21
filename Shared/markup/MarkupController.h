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

#ifndef MARKUPCONTROLLER_H
#define MARKUPCONTROLLER_H

// dsa app headers
#include "AbstractSketchTool.h"

// Qt headers
#include <QColor>

namespace Esri::ArcGISRuntime {
  enum class GeometryTypes;
}

namespace Dsa {

class MarkupBroadcast;

class MarkupController : public AbstractSketchTool
{
  Q_OBJECT

  Q_PROPERTY(bool is3d READ is3d NOTIFY is3dChanged)
  Q_PROPERTY(bool drawModeEnabled READ drawModeEnabled WRITE setDrawModeEnabled NOTIFY drawModeEnabledChanged)
  Q_PROPERTY(double drawingAltitude READ drawingAltitude WRITE setDrawingAltitude NOTIFY drawingAltitudeChanged)
  Q_PROPERTY(QStringList colors READ colors CONSTANT)

public:
  explicit MarkupController(QObject* parent = nullptr);
  ~MarkupController();

  void setProperties(const QVariantMap& properties) override;

  Q_INVOKABLE void setColor(const QColor& color);
  Q_INVOKABLE void setWidth(float width);
  Q_INVOKABLE void setSurfacePlacement(int placementEnum);
  Q_INVOKABLE void deleteSelectedGraphics();
  Q_INVOKABLE void deleteAllGraphics();
  Q_INVOKABLE void setOverlayName(const QString& name);
  Q_INVOKABLE void setActive(bool active) override;
  Q_INVOKABLE void shareMarkup();

  void setDrawingAltitude(double altitude);
  double drawingAltitude() const;

  bool drawModeEnabled() const;
  void setDrawModeEnabled(bool enabled);

  bool is3d() const;
  int sketchCount() const;
  QString toolName() const override;
  Esri::ArcGISRuntime::GeometryType geometryType() const override;
  QColor currentColor() const;

signals:
  void is3dChanged();
  void drawModeEnabledChanged();
  void drawingAltitudeChanged();
  void sketchCompleted();
  void sketchingChanged();
  void markupReceived(const QString& filePath, const QString& sharedBy);
  void markupSent(const QString& filePath);

private:
  void updateGeoView();
  void init();
  void updateSketch() override;
  Esri::ArcGISRuntime::Symbol* updatedSymbol();
  QStringList colors() const;

  static const QString USERNAME_PROPERTYNAME;
  int m_currentPartIndex = 0;
  double m_drawingAltitude = 10.0;
  bool m_isDrawing = false;
  bool m_drawModeEnabled = true;
  bool m_is3d = false;
  QColor m_color = QColor("black");
  QString m_username;
  float m_width = 8.0f;
  MarkupBroadcast* m_markupBroadcast = nullptr;
};

} // Dsa

#endif // MARKUPCONTROLLER_H
