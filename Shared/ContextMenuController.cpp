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

#include "ContextMenuController.h"
#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include <QDebug>

using namespace Esri::ArcGISRuntime;

/*!
  \brief Constructor accepting an optional \a parent.
 */
ContextMenuController::ContextMenuController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent),
  m_options(new QStringListModel(this))
{
  Toolkit::ToolManager::instance().addTool(this);

  // setup connection to handle mouse-clicking in the view (used to trigger the identify tasks)
  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mousePressedAndHeld,
          this, &ContextMenuController::onMousePressedAndHeld);
}

/*!
  \brief Destructor.
 */
ContextMenuController::~ContextMenuController()
{
}

/*!
  \brief The name of this tool.
 */
QString ContextMenuController::toolName() const
{
  return QStringLiteral("context menu");
}

/*!
  \brief Handles a mouse-click event in the view - used to trigger identify graphics and features tasks.
 */
void ContextMenuController::onMousePressedAndHeld(QMouseEvent& event)
{
  // ignore the event if the tool is not active.
//  if (!isActive())
//    return;

  // only consider left clicks (or taps) onn the view.
  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  clearOptions();

  setContextScreenPosition(event.pos());
  setContextActive(true);

  addOption(QStringLiteral("get elevation"));
  addOption(QStringLiteral("identify"));

  // accept the event to prevent it being used by other tools etc.
  event.accept();
}

void ContextMenuController::setContextScreenPosition(const QPoint& contextScreenPosition)
{
  if (m_contextScreenPosition == contextScreenPosition)
    return;

  m_contextScreenPosition = contextScreenPosition;
  emit contextScreenPositionChanged();
}

void ContextMenuController::addOption(const QString &option)
{
  QStringList options = m_options->stringList();
  if (options.contains(option))
    return;

  options.append(option);
  m_options->setStringList(options);
}

void ContextMenuController::clearOptions()
{
  m_options->setStringList(QStringList());
}

QStringListModel* ContextMenuController::options() const
{
  return m_options;
}

void ContextMenuController::selectOption(const QString& option)
{
  qDebug() << option;
  setContextActive(false);
}

QPoint ContextMenuController::contextScreenPosition() const
{
  return m_contextScreenPosition;
}

bool ContextMenuController::contextActive() const
{
  return m_contextActive;
}

void ContextMenuController::setContextActive(bool contextRequested)
{
  if (m_contextActive == contextRequested)
    return;

  m_contextActive = contextRequested;
  emit contextActiveChanged();
}
