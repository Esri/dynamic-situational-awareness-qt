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

// PCH header
#include "pch.hpp"

#include "MarkupUtility.h"

// example app headers

// toolkit headers

// C++ API headers
#include "GraphicsOverlay.h"
#include "SimpleLineSymbol.h"

// Qt headers
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QDebug>

using namespace Esri::ArcGISRuntime;

const QString MarkupUtility::USERNAME_PROPERTYNAME = "UserName";

/*
 \brief Constructor that takes an optional \a parent.
 */
MarkupUtility::MarkupUtility(QObject *parent) :
  Toolkit::AbstractTool(parent)
{
  Toolkit::ToolManager::instance().addTool(this);
}

/*
 \brief Destructor
 */
MarkupUtility::~MarkupUtility()
{
}

/*
 \brief Returns the tool's name
 */
QString MarkupUtility::toolName() const
{
  return QStringLiteral("Markup Utility");
}

/*
 \brief Sets \a properties from the configuration file
 */
void MarkupUtility::setProperties(const QVariantMap& properties)
{
  m_username = properties[USERNAME_PROPERTYNAME].toString();
}

/*
 \brief Convers the input \a graphicsOverlay to \c .markup JSON.
 */
QJsonObject MarkupUtility::graphicsToJson(GraphicsOverlay* graphicsOverlay)
{
  // get the sceneview instance
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());

  // create the markup
  QJsonObject markup;
  QJsonArray elements;
  for (int i = 0; i < graphicsOverlay->graphics()->size(); i++)
  {
    Graphic* graphic = graphicsOverlay->graphics()->at(i);
    QJsonObject element;
    QJsonDocument geomDoc = QJsonDocument::fromJson(graphic->geometry().toJson().toUtf8());
    element["geometry"] = QJsonValue(geomDoc.object());
    element["filled"] = false;
    element["arrow"] = false;
    SimpleLineSymbol* sls = dynamic_cast<SimpleLineSymbol*>(graphic->symbol());
    element["color"] = sls ? colors().indexOf(sls->color().name()) : 0;
    QJsonValue value(element);
    elements.append(value);
  }
  markup["elements"] = elements;
  markup["version"] = "1.0";
  markup["name"] = graphicsOverlay->overlayId();

  // create the markup item json
  QJsonObject markupJson;

  // set center
  markupJson["scale"] = sceneView ? (int)sceneView->currentViewpointCamera().location().z() : -1;
  markupJson["version"] = "1.0";
  QJsonDocument centerDoc = sceneView ?
        QJsonDocument::fromJson(sceneView->currentViewpointCamera().location().toJson().toUtf8()) :
        QJsonDocument();
  markupJson["center"] = sceneView ? QJsonValue(centerDoc.object()) : -1;

  // add the markups to the markup item json
  markupJson["markup"] = markup;

  // add the name of the sharer
  markupJson["sharedBy"] = m_username;

  qDebug() << markupJson;

  return markupJson;
}

QStringList MarkupUtility::colors() const
{
  return QStringList{QStringLiteral("#ff0000"), QStringLiteral("#ffd700"),
        QStringLiteral("#32cd32"), QStringLiteral("#00ffff"),
        QStringLiteral("#800080"), QStringLiteral("#ff00ff")};
}

