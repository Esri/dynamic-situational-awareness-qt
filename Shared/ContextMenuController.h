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

#ifndef CONTEXTMENUCONTROLLER_H
#define CONTEXTMENUCONTROLLER_H

#include "AbstractTool.h"

#include "Point.h"
#include "TaskWatcher.h"

#include <QMouseEvent>
#include <QStringListModel>

namespace Esri
{
namespace ArcGISRuntime
{
  class GeoElement;
  class IdentifyGraphicsOverlayResult;
  class IdentifyLayerResult;
}
}

class ContextMenuController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
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
  static const QString VIEWSHED_OPTION;

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
  void contextActiveChanged() const;
  void contextScreenPositionChanged() const;
  void optionsChanged() const;
  void resultChanged() const;
  void resultTitleChanged() const;

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

  bool m_contextActive = false;
  QPoint m_contextScreenPosition{0, 0};
  QStringListModel* m_options;
  QString m_result;
  QString m_resultTitle;
  Esri::ArcGISRuntime::Point m_contextLocation;
  Esri::ArcGISRuntime::TaskWatcher m_identifyFeaturesTask;
  Esri::ArcGISRuntime::TaskWatcher m_identifyGraphicsTask;
  Esri::ArcGISRuntime::TaskWatcher m_screenToLocationTask;
  Esri::ArcGISRuntime::GeoElement* m_contextElement = nullptr;
};

#endif // CONTEXTMENUCONTROLLER_H
