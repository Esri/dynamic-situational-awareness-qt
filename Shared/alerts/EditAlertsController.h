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

#ifndef EDITALERTSCONTROLLER_H
#define EDITALERTSCONTROLLER_H

#include "AbstractTool.h"

#include "TaskWatcher.h"

#include <QStringListModel>

class QMouseEvent;
class QStringList;

namespace Esri
{
namespace ArcGISRuntime
{
class IdentifyLayerResult;
class IdentifyGraphicsOverlayResult;
}
}

class EditAlertsController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
  Q_PROPERTY(QAbstractItemModel* layerNames READ layerNames NOTIFY layerNamesChanged)
  Q_PROPERTY(QAbstractItemModel* levelNames READ levelNames CONSTANT)
  Q_PROPERTY(QAbstractItemModel* conditionsList READ conditionsList NOTIFY conditionsListChanged)
  Q_PROPERTY(bool pickMode READ pickMode NOTIFY pickModeChanged)

public:
  explicit EditAlertsController(QObject* parent = nullptr);
  ~EditAlertsController();

  // AbstractTool interface
  QString toolName() const override;

  void setActive(bool active) override;

  Q_INVOKABLE void addWithinDistanceAlert(int levelIndex, int sourceOverlayIndex, double distance, int itemId, int targetOverlayIndex);
  Q_INVOKABLE void addIntersectsAlert(int levelIndex, int sourceOverlayIndex, int itemId, int targetOverlayIndex);
  Q_INVOKABLE void removeConditionAt(int rowIndex);
  Q_INVOKABLE void togglePickMode();

  QAbstractItemModel* layerNames() const;
  QAbstractItemModel* levelNames() const;
  QAbstractItemModel* conditionsList() const;
  bool pickMode() const;

signals:
  void activeChanged();
  void layerNamesChanged();
  void conditionsListChanged();
  void pickModeChanged();
  void pickedElement(const QString& overlayName, int elementId);

private slots:
  void onGeoviewChanged();
  void onLayersChanged();
  void onMouseClicked(QMouseEvent& event);
  void onIdentifyLayersCompleted(const QUuid& taskId, QList<Esri::ArcGISRuntime::IdentifyLayerResult*> identifyResults);
  void onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*> identifyResults);

private:
  void setLayerNames(const QStringList& layerNames);

  QStringListModel* m_layerNames;
  QStringListModel* m_levelNames;
  bool m_pickMode = false;
  double m_tolerance = 5;
  Esri::ArcGISRuntime::TaskWatcher m_identifyLayersWatcher;
  Esri::ArcGISRuntime::TaskWatcher m_identifyGraphicsWatcher;

  QMetaObject::Connection m_mouseClickConnection;
  QMetaObject::Connection m_identifyLayersConnection;
  QMetaObject::Connection m_identifyGraphicsConnection;
};

#endif // EDITALERTSCONTROLLER_H
