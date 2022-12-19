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

#ifndef CONTEXTMENUCONTROLLER_H
#define CONTEXTMENUCONTROLLER_H

// toolkit headers
#include "AbstractTool.h"

// C++ API headers
#include "Point.h"
#include "TaskWatcher.h"

// Qt headers
#include <QMouseEvent>
#include <QStringListModel>
#include <QHash>

namespace Esri {
namespace ArcGISRuntime {
  class GeoElement;
  class IdentifyGraphicsOverlayResult;
  class IdentifyLayerResult;
}
}

namespace Dsa {

class ContextMenuController : public AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool contextActive READ isContextActive WRITE setContextActive NOTIFY contextActiveChanged)
  Q_PROPERTY(QPoint contextScreenPosition READ contextScreenPosition NOTIFY contextScreenPositionChanged)
  Q_PROPERTY(QAbstractItemModel* options READ options NOTIFY optionsChanged)
  Q_PROPERTY(QString result READ result NOTIFY resultChanged)
  Q_PROPERTY(QString resultTitle READ resultTitle NOTIFY resultTitleChanged)

public:
  static const QString COORDINATES_OPTION;
  static const QString ELEVATION_OPTION;
  static const QString FOLLOW_OPTION;
  static const QString IDENTIFY_OPTION;
  static const QString LINE_OF_SIGHT_OPTION;
  static const QString VIEWSHED_OPTION;
  static const QString OBSERVATION_REPORT_OPTION;

  explicit ContextMenuController(QObject* parent = nullptr);
  ~ContextMenuController();

  QString toolName() const override;

  bool isContextActive() const;
  void setContextActive(bool contexActive);

  QPoint contextScreenPosition() const;
  void setContextScreenPosition(const QPoint& contextScreenPosition);

  QStringListModel* options() const;

  Q_INVOKABLE void selectOption(const QString& option);

  QString result() const;
  QString resultTitle() const;

  Esri::ArcGISRuntime::Point contextLocation() const;
  void setContextLocation(const Esri::ArcGISRuntime::Point& location);

signals:
  void contextActiveChanged();
  void contextScreenPositionChanged();
  void optionsChanged();
  void resultChanged();
  void resultTitleChanged();

private slots:
  void onMousePressedAndHeld(QMouseEvent& event);
  void onIdentifyLayersCompleted(const QUuid& taskId, const QList<Esri::ArcGISRuntime::IdentifyLayerResult*>& identifyResults);
  void onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, const QList<Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult*>& identifyResults);
  void onScreenToLocationCompleted(QUuid taskId, const Esri::ArcGISRuntime::Point& location);

private:
  void addOption(const QString& option);
  void clearOptions();
  void setResult(const QString& result);
  void setResultTitle(const QString& resultTitle);
  void cancelTasks();
  void cancelIdentifyTasks();
  void processGeoElements();

  bool m_contextActive = false;
  QPoint m_contextScreenPosition{0, 0};
  QStringListModel* m_options;
  QString m_result;
  QString m_resultTitle;
  Esri::ArcGISRuntime::Point m_contextLocation;
  Esri::ArcGISRuntime::Point m_contextBaseSurfaceLocation;
  Esri::ArcGISRuntime::TaskWatcher m_identifyFeaturesTask;
  Esri::ArcGISRuntime::TaskWatcher m_identifyGraphicsTask;
  Esri::ArcGISRuntime::TaskWatcher m_screenToLocationTask;
  QHash<QString, QList<Esri::ArcGISRuntime::GeoElement*>> m_contextFeatures;
  QHash<QString, QList<Esri::ArcGISRuntime::GeoElement*>> m_contextGraphics;
};

} // Dsa

#endif // CONTEXTMENUCONTROLLER_H
