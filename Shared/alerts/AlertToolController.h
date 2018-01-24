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

#ifndef ALERTTOOLCONTROLLER_H
#define ALERTTOOLCONTROLLER_H

#include <QAbstractListModel>

#include "AbstractTool.h"

class AlertQuery;
class AlertListProxyModel;
class IntersectsAlertRule;
class IdsAlertRule;
class WithinDistanceAlertQuery;
class StatusAlertRule;

namespace Esri {
namespace ArcGISRuntime
{
  class GraphicsOverlay;
  class SimpleMarkerSceneSymbol;
}}

class QTimer;

class GeoElementHighlighter;

class AlertToolController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* alertListModel READ alertListModel NOTIFY alertListModelChanged)

public:
  explicit AlertToolController(QObject* parent = nullptr);
  ~AlertToolController();

  QAbstractItemModel* alertListModel() const;

  // AbstractTool interface
  QString toolName() const override;

  Q_INVOKABLE void highlight(int rowIndex, bool showHighlight);
  Q_INVOKABLE void zoomTo(int rowIndex);
  Q_INVOKABLE void setViewed(int rowIndex);
  Q_INVOKABLE void dismiss(int rowIndex);
  Q_INVOKABLE void setMinLevel(int level);

  Q_INVOKABLE void flashAll(bool on);

signals:
  void alertListModelChanged();

private:
  AlertListProxyModel* m_alertsProxyModel = nullptr;
  WithinDistanceAlertQuery* m_distanceAlertRule = nullptr;
  IntersectsAlertRule* m_intersectsRule = nullptr;
  StatusAlertRule* m_statusAlertRule = nullptr;
  IdsAlertRule* m_idsAlertRule = nullptr;
  QList<AlertQuery*> m_rules;
  GeoElementHighlighter* m_highlighter = nullptr;

  QMetaObject::Connection m_highlightConnection;
};

#endif // ALERTTOOLCONTROLLER_H
