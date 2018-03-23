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

// Qt headers
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QDebug>

using namespace Esri::ArcGISRuntime;

QJsonObject MarkupUtility::graphicsToJson(GraphicsOverlay* graphicsOverlay, const QString& name)
{
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
    element["color"] = 0; // TODO - get color index
    QJsonValue value(element);
    elements.append(value);
  }
  markup["elements"] = elements;
  markup["version"] = "1.0";
  markup["name"] = name;

  // create the markup item json
  QJsonObject markupJson;
  // set center
  markupJson["scale"] = 10000; // TODO - get scale
  markupJson["version"] = "1.0";
  QJsonDocument centerDoc = QJsonDocument::fromJson(Point(0,0,SpatialReference(3857)).toJson().toUtf8());
  markupJson["center"] = QJsonValue(centerDoc.object()); // TODO - get center of scene/graphics

  // add the markups to the markup item json
  markupJson["markup"] = markup;

  // add the name of the sharer
  markupJson["sharedBy"] = "Lucas"; // TODO - get name from options

  qDebug() << markupJson;

  return markupJson;
}

