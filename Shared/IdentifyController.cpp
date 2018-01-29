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

#include "IdentifyController.h"

#include "ToolManager.h"
#include "ToolResourceProvider.h"

#include "GeoElement.h"
#include "GeoView.h"
#include "Graphic.h"
#include "Popup.h"
#include "PopupManager.h"

using namespace Esri::ArcGISRuntime;

struct LayerResultsManager {

  QList<IdentifyLayerResult*>& m_results;

  LayerResultsManager(QList<IdentifyLayerResult*>& results):
    m_results(results)
  {
  }

  ~LayerResultsManager()
  {
    qDeleteAll(m_results);
  }
};

struct GraphicsOverlaysResultsManager {

  QList<IdentifyGraphicsOverlayResult*>& m_results;

  GraphicsOverlaysResultsManager(QList<IdentifyGraphicsOverlayResult*>& results):
    m_results(results)
  {
  }

  ~GraphicsOverlaysResultsManager()
  {
    qDeleteAll(m_results);
  }
};

IdentifyController::IdentifyController(QObject* parent /* = nullptr */):
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::mouseClicked,
          this, &IdentifyController::onMouseClicked);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyLayersCompleted,
          this, &IdentifyController::onIdentifyLayersCompleted);

  connect(Toolkit::ToolResourceProvider::instance(), &Toolkit::ToolResourceProvider::identifyGraphicsOverlaysCompleted,
          this, &IdentifyController::onIdentifyGraphicsOverlaysCompleted);
}

IdentifyController::~IdentifyController()
{
}

QString IdentifyController::toolName() const
{
  return QStringLiteral("identify");
}

void IdentifyController::setActive(bool active)
{
  if (active == m_active)
    return;

  if (busy())
  {
    m_layersWatcher.cancel();
    m_graphicsOverlaysWatcher.cancel();
  }

  m_active = active;
  emit activeChanged();
}

bool IdentifyController::busy() const
{
  return (m_layersWatcher.isValid() && !m_layersWatcher.isDone() && !m_layersWatcher.isCanceled()) ||
      (m_graphicsOverlaysWatcher.isValid() && !m_graphicsOverlaysWatcher.isDone() && !m_graphicsOverlaysWatcher.isCanceled());
}

QVariantList IdentifyController::popupManagers() const
{
  QVariantList res;

  for (PopupManager* mgr : m_popupManagers)
  {
    QVariant v = QVariant::fromValue(mgr);
    res.push_back(v);
  }

  return res;
}

void IdentifyController::onMouseClicked(QMouseEvent& event)
{
  if (!isActive())
    return;

  if (event.button() != Qt::MouseButton::LeftButton)
    return;

  if (busy())
    return;

  GeoView* geoView = Toolkit::ToolResourceProvider::instance()->geoView();
  if (!geoView)
    return;

  m_layersWatcher = geoView->identifyLayers(event.pos().x(), event.pos().y(), m_tolerance, false);
  m_graphicsOverlaysWatcher = geoView->identifyGraphicsOverlays(event.pos().x(), event.pos().y(), m_tolerance, false);
  emit busyChanged();

  m_popupManagers.clear();
  emit popupManagersChanged();

  event.accept();
}

void IdentifyController::onIdentifyLayersCompleted(const QUuid& taskId, QList<IdentifyLayerResult*> identifyResults)
{
  if (taskId != m_layersWatcher.taskId())
    return;

  LayerResultsManager resultsManager(identifyResults);

  m_layersWatcher = TaskWatcher();
  emit busyChanged();

  if (!isActive())
    return;

  bool anyAdded = false;
  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyLayerResult* res = *it;
    if (!res)
      continue;

    const QString resTitle = res->layerContent()->name();
    const QList<GeoElement*> geoElements = res->geoElements();
    for(GeoElement* g : geoElements)
    {
      if (addGeoElementPopup(g, resTitle))
        anyAdded = true;
    }
  }

  if (anyAdded)
    emit popupManagersChanged();
}

void IdentifyController::onIdentifyGraphicsOverlaysCompleted(const QUuid& taskId, QList<IdentifyGraphicsOverlayResult*> identifyResults)
{
  if (taskId != m_graphicsOverlaysWatcher.taskId())
    return;

  GraphicsOverlaysResultsManager resultsManager(identifyResults);

  m_graphicsOverlaysWatcher = TaskWatcher();
  emit busyChanged();

  if (!isActive())
    return;

  bool anyAdded = false;
  auto it = resultsManager.m_results.begin();
  auto itEnd = resultsManager.m_results.end();
  for (; it != itEnd; ++it)
  {
    IdentifyGraphicsOverlayResult* res = *it;
    if (!res)
      continue;

    const QString resTitle = res->graphicsOverlay()->overlayId();
    const QList<Graphic*> graphics = res->graphics();

    for(Graphic* g : graphics)
    {
      if (addGeoElementPopup(g, resTitle))
        anyAdded = true;
    }
  }

  if (anyAdded)
    emit popupManagersChanged();
}

bool IdentifyController::addGeoElementPopup(GeoElement* geoElement, const QString& popupTitle)
{
  if (!geoElement)
    return false;

  if (!geoElement->attributes() || geoElement->attributes()->isEmpty())
    return false;

  Popup* newPopup = new Popup(geoElement, this);
  newPopup->popupDefinition()->setTitle(popupTitle);
  PopupManager* newManager = new PopupManager(newPopup, this);

  m_popupManagers.push_back(newManager);

  return true;
}
