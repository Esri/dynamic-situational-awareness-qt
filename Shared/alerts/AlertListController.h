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

#ifndef ALERTLISTCONTROLLER_H
#define ALERTLISTCONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// Qt headers
#include <QAbstractListModel>

namespace Esri {
namespace ArcGISRuntime
{
  class GraphicsOverlay;
  class SimpleMarkerSceneSymbol;
}}

namespace Dsa {

class PointHighlighter;

class AlertFilter;
class AlertListProxyModel;
class IdsAlertFilter;
class StatusAlertFilter;

class AlertListController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(QAbstractItemModel* alertListModel READ alertListModel NOTIFY alertListModelChanged)
  Q_PROPERTY(int allAlertsCount READ allAlertsCount NOTIFY allAlertsCountChanged)

public:
  explicit AlertListController(QObject* parent = nullptr);
  ~AlertListController();

  QAbstractItemModel* alertListModel() const;
  int allAlertsCount() const;

  // AbstractTool interface
  QString toolName() const override;

  Q_INVOKABLE void highlight(int rowIndex, bool showHighlight);
  Q_INVOKABLE void zoomTo(int rowIndex);
  Q_INVOKABLE void setViewed(int rowIndex);
  Q_INVOKABLE void dismiss(int rowIndex);
  Q_INVOKABLE void setMinLevel(int level);
  Q_INVOKABLE void clearAllFilters();

  Q_INVOKABLE void flashAll(bool highlight);

signals:
  void alertListModelChanged();
  void allAlertsCountChanged();
  void highlightStopped();

private:
  AlertListProxyModel* m_alertsProxyModel = nullptr;
  StatusAlertFilter* m_statusAlertFilter = nullptr;
  IdsAlertFilter* m_idsAlertFilter = nullptr;
  QList<AlertFilter*> m_filters;
  PointHighlighter* m_highlighter = nullptr;

  QList<QMetaObject::Connection> m_highlightConnections;
};

} // Dsa

#endif // ALERTLISTCONTROLLER_H
