// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

#include <QUrl>

#include "AbstractTool.h"

namespace Esri {
namespace ArcGISRuntime
{
  class SceneView;
  class GeoView;
}}

class NavigationController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool vertical READ isVertical NOTIFY verticalChanged)

public:

  explicit NavigationController(QObject* parent = nullptr);
  ~NavigationController();

  Q_INVOKABLE void zoomOut();
  Q_INVOKABLE void zoomIn();
  Q_INVOKABLE void tilt();

  QString toolName() const override;
  void setProperties(const QVariantMap& properties) override;

signals:
  void verticalChanged();

private slots:
  void updateGeoView();

private:
  bool isVertical() { return m_isCameraVertical; }
  void zoom(double zoomFactor);

  Esri::ArcGISRuntime::GeoView* m_geoView   = nullptr;
  Esri::ArcGISRuntime::SceneView* m_sceneView = nullptr;
  bool m_is3d                               = false;
  bool m_isCameraVertical                   = false;
};

#endif // NAVIGATIONCONTROLLER_H
