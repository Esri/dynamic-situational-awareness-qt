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

#ifndef LINEOFSIGHTCONTROLLER_H
#define LINEOFSIGHTCONTROLLER_H

#include "AbstractTool.h"

#include "Point.h"
#include "TaskWatcher.h"

#include <QAbstractItemModel>

namespace Esri
{
namespace ArcGISRuntime
{
  class AnalysisOverlay;
  class GeoElement;
  class GeoView;
  class LayerListModel;
  class FeatureLayer;
  class FeatureQueryResult;
}
}

class QStringListModel;

class LineOfSightController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* overlayNames READ overlayNames NOTIFY overlayNamesChanged)
  Q_PROPERTY(bool analysisVisible READ isAnalysisVisible WRITE setAnalysisVisible NOTIFY analysisVisibleChanged)

public:

  explicit LineOfSightController(QObject* parent = nullptr);
  ~LineOfSightController();

  QString toolName() const override;
  void setActive(bool active) override;

  QAbstractItemModel* overlayNames() const;

  Q_INVOKABLE void selectOverlayIndex(int overlayIndex);
  Q_INVOKABLE void clearAnalysis();

  bool isAnalysisVisible() const;
  void setAnalysisVisible(bool isAnalysisVisiblesVisible);

  Esri::ArcGISRuntime::AnalysisOverlay* lineOfSightOverlay() const;

signals:
  void toolErrorOccurred(const QString& errorMessage, const QString& additionalMessage);
  void overlayNamesChanged();
  void analysisVisibleChanged();

public slots:
  void onGeoViewChanged(Esri::ArcGISRuntime::GeoView* geoView);
  void onOperationalLayersChanged(Esri::ArcGISRuntime::LayerListModel* operationalLayers);

private slots:
  void onQueryFeaturesCompleted(QUuid taskId, Esri::ArcGISRuntime::FeatureQueryResult* featureQueryResult);

private:
  void cancelTask();
  void getLocationGeoElement();

  QStringListModel* m_overlayNames;
  Esri::ArcGISRuntime::GeoView* m_geoView = nullptr;
  QList<Esri::ArcGISRuntime::FeatureLayer*> m_overlays;
  Esri::ArcGISRuntime::AnalysisOverlay* m_lineOfSightOverlay = nullptr;
  QObject* m_lineOfSightParent = nullptr;
  Esri::ArcGISRuntime::TaskWatcher m_featuresTask;
  Esri::ArcGISRuntime::GeoElement* m_locationGeoElement = nullptr;
  QMetaObject::Connection m_queryFeaturesConnection;
  bool m_analysisVisible = true;
};

#endif // LINEOFSIGHTCONTROLLER_H
