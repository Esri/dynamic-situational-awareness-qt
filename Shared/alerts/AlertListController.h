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

#ifndef ALERTLISTCONTROLLER_H
#define ALERTLISTCONTROLLER_H

#include <QAbstractListModel>

#include "AbstractTool.h"

class AlertFilter;
class AlertListProxyModel;
class IdsAlertFilter;
class StatusAlertFilter;

namespace Esri {
namespace ArcGISRuntime
{
  class GraphicsOverlay;
  class SimpleMarkerSceneSymbol;
}}

class QTimer;

class PointHighlighter;

class AlertListController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* alertListModel READ alertListModel NOTIFY alertListModelChanged)

public:
  explicit AlertListController(QObject* parent = nullptr);
  ~AlertListController();

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
  StatusAlertFilter* m_statusAlertFilter = nullptr;
  IdsAlertFilter* m_idsAlertFilter = nullptr;
  QList<AlertFilter*> m_filters;
  PointHighlighter* m_highlighter = nullptr;

  QList<QMetaObject::Connection> m_highlightConnections;
};

#endif // ALERTLISTCONTROLLER_H
