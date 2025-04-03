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

#ifndef ANALYSISLISTCONTROLLER_H
#define ANALYSISLISTCONTROLLER_H

// Qt headers
#include <qtmetamacros.h>
class QAbstractItemModel;
Q_MOC_INCLUDE("qabstractitemmodel.h")

// dsa headers
#include "AbstractTool.h"

namespace Esri::ArcGISRuntime {
  class GeoView;
  class Point;
  class SceneView;
}

namespace Dsa {

class CombinedAnalysisListModel;

class AnalysisListController : public AbstractTool
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

} // Dsa

#endif // ANALYSISLISTCONTROLLER_H
