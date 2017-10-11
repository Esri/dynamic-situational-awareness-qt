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

public:
  explicit TelestrateController(QObject* parent = nullptr);
  ~TelestrateController();

  Q_INVOKABLE void setActive(bool active) override;
  Q_INVOKABLE void setColor(QColor color);
  Q_INVOKABLE void setSurfacePlacement(int placementEnum);
  Q_INVOKABLE void deleteSelectedGraphics();
  Q_INVOKABLE void deleteAllGraphics();
  Q_INVOKABLE void setDrawMode(bool enabled);
  Q_INVOKABLE bool is3d() const;

  QString toolName() const override;
  Esri::ArcGISRuntime::GeometryType geometryType() const override;

  void updateGeoView();
  void init(Esri::ArcGISRuntime::SceneQuickView* sceneView);
  void init(Esri::ArcGISRuntime::MapQuickView* mapView);
  void refreshSketchLayer();

  void updateSketch() override;

signals:
  void is3dChanged();

private:
  int m_currentPartIndex = 0;
  bool m_isDrawing = false;
  bool m_drawingModeOn = true;
  bool m_is3d = false;
};

#endif // TELESTRATECONTROLLER_H
