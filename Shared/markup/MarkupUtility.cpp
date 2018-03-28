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

// example app headers
#include "MarkupUtility.h"
#include "MarkupLayer.h"

// C++ API headers
#include "GraphicsOverlay.h"
#include "SimpleLineSymbol.h"
#include "FeatureCollection.h"
#include "FeatureCollectionLayer.h"
#include "FeatureCollectionTable.h"
#include "Feature.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"

// Qt headers
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QHash>
#include <QDebug>

using namespace Esri::ArcGISRuntime;

const QString MarkupUtility::ARROW = QStringLiteral("arrow");
const QString MarkupUtility::CENTER = QStringLiteral("center");
const QString MarkupUtility::COLOR = QStringLiteral("color");
const QString MarkupUtility::ELEMENTS = QStringLiteral("elements");
const QString MarkupUtility::FILLED = QStringLiteral("filled");
const QString MarkupUtility::GEOMETRY = QStringLiteral("geometry");
const QString MarkupUtility::MARKUP = QStringLiteral("markup");
const QString MarkupUtility::NAME = QStringLiteral("name");
const QString MarkupUtility::SCALE = QStringLiteral("scale");
const QString MarkupUtility::SHAREDBY = QStringLiteral("sharedBy");
const QString MarkupUtility::USERNAME_PROPERTYNAME = QStringLiteral("UserName");
const QString MarkupUtility::VERSION = QStringLiteral("version");
const QString MarkupUtility::VERSIONNUMBER = QStringLiteral("1.0");

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
 \brief Converts the input \a graphicsOverlay to \c .markup JSON.
 */
QJsonObject MarkupUtility::graphicsToJson(GraphicsOverlay* graphicsOverlay)
{
  // get the sceneview instance
  SceneView* sceneView = dynamic_cast<SceneView*>(Toolkit::ToolResourceProvider::instance()->geoView());

  // create the markup
  QJsonObject markup;
  QJsonArray elements;
  const int graphicCount = graphicsOverlay->graphics()->size();
  for (int i = 0; i < graphicCount; i++)
  {
    Graphic* graphic = graphicsOverlay->graphics()->at(i);
    QJsonObject element;
    QJsonDocument geomDoc = QJsonDocument::fromJson(graphic->geometry().toJson().toUtf8());
    element[GEOMETRY] = QJsonValue(geomDoc.object());
    element[FILLED] = false;
    element[ARROW] = false;
    SimpleLineSymbol* sls = dynamic_cast<SimpleLineSymbol*>(graphic->symbol());
    element[COLOR] = sls ? colors().indexOf(sls->color().name()) : 0;
    QJsonValue value(element);
    elements.append(value);
  }
  markup[ELEMENTS] = elements;
  markup[VERSION] = VERSIONNUMBER;
  markup[NAME] = graphicsOverlay->overlayId();

  // create the markup item json
  QJsonObject markupJson;

  // set center
  markupJson[SCALE] = sceneView ? (int)sceneView->currentViewpointCamera().location().z() : -1;
  markupJson[VERSION] = "1.0";
  QJsonDocument centerDoc = sceneView ?
        QJsonDocument::fromJson(sceneView->currentViewpointCamera().location().toJson().toUtf8()) :
        QJsonDocument();
  markupJson[CENTER] = sceneView ? QJsonValue(centerDoc.object()) : -1;

  // add the markups to the markup item json
  markupJson[MARKUP] = markup;

  // add the name of the sharer
  markupJson[SHAREDBY] = m_username;

  return markupJson;
}

/*
 \brief Returns a MarkupLayer for the input \a json.
*/
MarkupLayer* MarkupUtility::createMarkupLayer(const QString& json)
{
  const QJsonDocument markupDoc = QJsonDocument::fromJson(json.toUtf8());
  const QJsonObject markupJson = markupDoc.object();

  // Create the FeatureCollectionTable
  FeatureCollectionTable* table = new FeatureCollectionTable(QList<Field>{}, GeometryType::Polyline, SpatialReference(4326), true, false, this);
  SimpleRenderer* defaultRenderer = new SimpleRenderer(this);
  defaultRenderer->setSymbol(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 12.0f, this));
  table->setRenderer(defaultRenderer);

  // Clear Hash to keep track of features/symbols added to the table
  m_featureHash.clear();

  // Connect to know when addFeature successfully completes
  connect(table, &FeatureCollectionTable::addFeatureCompleted, this, [this, table](QUuid id, bool success)
  {
    if (!success || !m_featureHash.contains(id))
      return;

    auto pair = m_featureHash.value(id);
    table->setSymbolOverride(pair.first, pair.second);
  });

  // Loop through the markup elements and add them as Features to the table
  QJsonArray markupElements = markupJson.value(MARKUP).toObject().value(ELEMENTS).toArray();
  if (markupElements.isEmpty())
    return nullptr;

  int markupSize = markupElements.size();
  for (int i = 0; i < markupSize; i++)
  {
    const QJsonObject element = markupElements.at(i).toObject();
    Feature* feature = table->createFeature(table);
    const QString geomString = QString(QJsonDocument(element.value(GEOMETRY).toObject()).toJson(QJsonDocument::Compact));
    feature->setGeometry(Geometry::fromJson(geomString));
    QUuid id = table->addFeature(feature).taskId();
    auto symbol = createLineSymbolFromColor(element.value(COLOR).toInt());
    m_featureHash[id] = QPair<Feature*, SimpleLineSymbol*>(feature, symbol);
  }

  // Add the table to a Collection
  FeatureCollection* featureCollection = new FeatureCollection(QList<FeatureCollectionTable*>{table}, this);

  // Create a MarkupLayer
  MarkupLayer* markupLayer = new MarkupLayer(featureCollection, this);
  markupLayer->setName(markupJson.value(MARKUP).toObject().value(NAME).toString());

  return markupLayer;
}

/*
 \brief Returns a list of colors to be used in the Markup tool.
*/
QStringList MarkupUtility::colors() const
{
  return QStringList{QStringLiteral("#ff0000"), QStringLiteral("#ffd700"),
        QStringLiteral("#32cd32"), QStringLiteral("#00ffff"),
        QStringLiteral("#800080"), QStringLiteral("#ff00ff")};
}

/*
 \brief Returns a new SimpleLineSymbol for the specified \a index in the \l colors() list.
*/
SimpleLineSymbol* MarkupUtility::createLineSymbolFromColor(int index)
{
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(colors().at(index)), 12.0f, this);
  return sls;
}
