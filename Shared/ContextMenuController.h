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

#include <QMouseEvent>
#include <QStringListModel>

namespace Esri
{
namespace ArcGISRuntime
{
}
}

class ContextMenuController : public Esri::ArcGISRuntime::Toolkit::AbstractTool
{
  Q_OBJECT

  Q_PROPERTY(bool contextActive READ contextActive WRITE setContextActive NOTIFY contextActiveChanged)
  Q_PROPERTY(QPoint contextScreenPosition READ contextScreenPosition NOTIFY contextScreenPositionChanged)
  Q_PROPERTY(QAbstractItemModel* options READ options NOTIFY optionsChanged)
  Q_PROPERTY(QString result READ result NOTIFY resultChanged)

public:

  explicit ContextMenuController(QObject* parent = nullptr);
  ~ContextMenuController();

  QString toolName() const override;

  bool contextActive() const;
  void setContextActive(bool contexActive);

  QPoint contextScreenPosition() const;

  QStringListModel* options() const;

  Q_INVOKABLE void selectOption(const QString& option);

  QString result() const;
  void setResult(const QString &result);

signals:
  void contextActiveChanged() const;
  void contextScreenPositionChanged() const;
  void optionsChanged() const;
  void resultChanged() const;

private slots:
  void onMousePressedAndHeld(QMouseEvent& event);

private:
  void setContextScreenPosition(const QPoint &contextScreenPosition);
  void addOption(const QString& option);
  void clearOptions();

  bool m_contextActive = false;
  QPoint m_contextScreenPosition{0, 0};
  QStringListModel* m_options;
  QString m_result;
};

#endif // CONTEXTMENUCONTROLLER_H
