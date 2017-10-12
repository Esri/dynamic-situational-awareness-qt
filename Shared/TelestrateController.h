#ifndef TELESTRATECONTROLLER_H
#define TELESTRATECONTROLLER_H

#include "AbstractTool.h"
#include "SketchTool.h"

#include <QColor>

#include "GeometryTypes.h"

namespace Esri {
  namespace ArcGISRuntime {
    class MapQuickView;
    class SceneQuickView;
  }
}

class TelestrateController : public SketchTool
{
  Q_OBJECT

  Q_PROPERTY(bool is3d READ is3d NOTIFY is3dChanged)
  Q_PROPERTY(bool drawModeEnabled READ drawModeEnabled WRITE setDrawModeEnabled NOTIFY drawModeEnabledChanged)
  Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

public:
  explicit TelestrateController(QObject* parent = nullptr);
  ~TelestrateController();

  Q_INVOKABLE void setColor(QColor color);
  Q_INVOKABLE void setSurfacePlacement(int placementEnum);
  Q_INVOKABLE void deleteSelectedGraphics();
  Q_INVOKABLE void deleteAllGraphics();

  void setActive(bool active) override;

  bool drawModeEnabled() const;
  void setDrawModeEnabled(bool enabled);

  bool is3d() const;
  int sketchCount() const;
  QString toolName() const override;
  Esri::ArcGISRuntime::GeometryType geometryType() const override;

signals:
  void is3dChanged();
  void drawModeEnabledChanged();
  void activeChanged();

private:
  void updateGeoView();
  void init();
  void refreshSketchLayer();
  void updateSketch() override;

  int m_currentPartIndex = 0;
  bool m_isDrawing = false;
  bool m_drawModeEnabled = true;
  bool m_is3d = false;
};

#endif // TELESTRATECONTROLLER_H
