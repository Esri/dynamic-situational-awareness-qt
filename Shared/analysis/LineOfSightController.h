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

#ifndef LINEOFSIGHTCONTROLLER_H
#define LINEOFSIGHTCONTROLLER_H

// Qt headers
#include <QUuid>
class QAbstractItemModel;
Q_MOC_INCLUDE("qabstractitemmodel.h")

class QStringListModel;

// C++ API headers
#include "TaskWatcher.h"

// dsa headers
#include "AbstractTool.h"

namespace Esri::ArcGISRuntime {
  class AnalysisOverlay;
  class GeoElement;
  class GeoView;
  class LayerListModel;
  class FeatureLayer;
  class FeatureQueryResult;
}

namespace Dsa {

class LineOfSightController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* overlayNames READ overlayNames NOTIFY overlayNamesChanged)
  Q_PROPERTY(bool analysisVisible READ isAnalysisVisible WRITE setAnalysisVisible NOTIFY analysisVisibleChanged)
  Q_PROPERTY(int visibleByCount READ visibleByCount NOTIFY visibleByCountChanged)

public:

  explicit LineOfSightController(QObject* parent = nullptr);
  ~LineOfSightController();

  QString toolName() const override;
  void setActive(bool active) override;

  QAbstractItemModel* overlayNames() const;

  Q_INVOKABLE bool selectOverlayIndex(int overlayIndex);
  Q_INVOKABLE void clearAnalysis();

  bool isAnalysisVisible() const;
  void setAnalysisVisible(bool isAnalysisVisiblesVisible);

  Esri::ArcGISRuntime::AnalysisOverlay* lineOfSightOverlay() const;

  void lineOfSightFromLocationToGeoElement(Esri::ArcGISRuntime::GeoElement* geoElement);

  int visibleByCount() const;

signals:
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  void overlayNamesChanged();
  void analysisVisibleChanged();
  void visibleByCountChanged();

public slots:
  void onGeoViewChanged(Esri::ArcGISRuntime::GeoView* geoView);
  void onOperationalLayersChanged();

private slots:
  void onQueryFeaturesCompleted(QUuid taskId, Esri::ArcGISRuntime::FeatureQueryResult* featureQueryResult);

private:
  void cancelTask();
  void getLocationGeoElement();
  void setVisibleByCount(int visibleByCount);

  QStringListModel* m_overlayNames;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  QList<Esri::ArcGISRuntime::FeatureLayer*> m_overlays;
  Esri::ArcGISRuntime::AnalysisOverlay* m_lineOfSightOverlay = nullptr;
  QObject* m_lineOfSightParent = nullptr;
  Esri::ArcGISRuntime::TaskWatcher m_featuresTask;
  Esri::ArcGISRuntime::GeoElement* m_locationGeoElement = nullptr;
  QMetaObject::Connection m_queryFeaturesConnection;
  bool m_analysisVisible = true;
  int m_visibleByCount = 0;
  QList<QMetaObject::Connection> m_visibleByConnections;
};

} // Dsa

#endif // LINEOFSIGHTCONTROLLER_H
