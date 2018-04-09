// Copyright 2018 ESRI
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

#ifndef ANALYSISLISTCONTROLLER_H
#define ANALYSISLISTCONTROLLER_H


// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractItemModel>

namespace Esri {
namespace ArcGISRuntime {
  class GeoView;
  class Point;
  class SceneView;
}
}

namespace Dsa {
namespace Analysis {

class CombinedAnalysisListModel;

class AnalysisListController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* analysisList READ analysisList NOTIFY analysisListChanged)

public:

  explicit AnalysisListController(QObject* parent = nullptr);
  ~AnalysisListController();

  QString toolName() const override;

  QAbstractItemModel* analysisList() const;

  Q_INVOKABLE void removeAt(int index);
  Q_INVOKABLE void zoomTo(int index);

signals:
  void analysisListChanged();

public slots:
  void onGeoViewChanged(Esri::ArcGISRuntime::GeoView* geoView);

private:
  void zoomToLocation(const Esri::ArcGISRuntime::Point& location);

  CombinedAnalysisListModel* m_analysisList = nullptr;
  Esri::ArcGISRuntime::SceneView* m_sceneView = nullptr;
};

} // Analysis
} // Dsa

#endif // ANALYSISLISTCONTROLLER_H
