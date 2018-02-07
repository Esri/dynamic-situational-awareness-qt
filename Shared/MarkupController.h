#ifndef MARKUPCONTROLLER_H
#define MARKUPCONTROLLER_H

#include "AbstractTool.h"
#include "AbstractSketchTool.h"

#include <QColor>

#include "GeometryTypes.h"

class MarkupController : public AbstractSketchTool
{
  Q_OBJECT

  Q_PROPERTY(bool is3d READ is3d NOTIFY is3dChanged)
  Q_PROPERTY(bool drawModeEnabled READ drawModeEnabled WRITE setDrawModeEnabled NOTIFY drawModeEnabledChanged)
  Q_PROPERTY(double drawingAltitude READ drawingAltitude WRITE setDrawingAltitude NOTIFY drawingAltitudeChanged)

public:
  explicit MarkupController(QObject* parent = nullptr);
  ~MarkupController();

  Q_INVOKABLE void setColor(const QColor& color);
  Q_INVOKABLE void setWidth(float width);
  Q_INVOKABLE void setSurfacePlacement(int placementEnum);
  Q_INVOKABLE void deleteSelectedGraphics();
  Q_INVOKABLE void deleteAllGraphics();
  Q_INVOKABLE void setName(const QString& name);
  Q_INVOKABLE void clearGraphics();
  Q_INVOKABLE void setActive(bool active) override;
  Q_INVOKABLE void setIsSketching(bool isSketching);
  Q_INVOKABLE void clearCurrentSketch();

  void setDrawingAltitude(double altitude);
  double drawingAltitude() const;

  bool drawModeEnabled() const;
  void setDrawModeEnabled(bool enabled);

  bool is3d() const;
  int sketchCount() const;
  QString toolName() const override;
  Esri::ArcGISRuntime::GeometryType geometryType() const override;

signals:
  void is3dChanged();
  void drawModeEnabledChanged();
  void drawingAltitudeChanged();
  void sketchComplete();

private:
  void updateGeoView();
  void init();
  void updateSketch() override;
  void updateSymbol();

  int m_currentPartIndex = 0;
  double m_drawingAltitude = 10.0;
  bool m_isDrawing = false;
  bool m_drawModeEnabled = true;
  bool m_is3d = false;
  bool m_isSketching = false;
  QColor m_color = QColor("black");
  float m_width = 8.0f;
};

#endif // MARKUPCONTROLLER_H
