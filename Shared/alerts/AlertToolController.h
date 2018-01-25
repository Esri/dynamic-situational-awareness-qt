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
class WithinAreaAlertQuery;
class IdsAlertQuery;
class WithinDistanceAlertQuery;
class StatusAlertQuery;

namespace Esri {
namespace ArcGISRuntime
{
  class GraphicsOverlay;
  class SimpleMarkerSceneSymbol;
}}

class QTimer;

class PointHighlighter;

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
  WithinDistanceAlertQuery* m_withinDistanceAlertRule = nullptr;
  WithinAreaAlertQuery* m_withinAreaRule = nullptr;
  StatusAlertQuery* m_statusAlertRule = nullptr;
  IdsAlertQuery* m_idsAlertRule = nullptr;
  QList<AlertQuery*> m_rules;
  PointHighlighter* m_highlighter = nullptr;

  QList<QMetaObject::Connection> m_highlightConnections;
};

#endif // ALERTTOOLCONTROLLER_H
