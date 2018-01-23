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

#include "FeatureOverlayManager.h"

#include "Feature.h"
#include "FeatureLayer.h"

#include <QDebug>
#include <QEventLoop>
#include <QThread>
#include <QTimer>

using namespace Esri::ArcGISRuntime;

FeatureOverlayManager::FeatureOverlayManager(FeatureLayer* overlay, QObject* parent):
  AbstractOverlayManager(parent),
  m_overlay(overlay)
{

}

FeatureOverlayManager::~FeatureOverlayManager()
{

}

void FeatureOverlayManager::setSelected(GeoElement* element, bool on)
{
  if (!element)
    return;

  Feature* f = qobject_cast<Feature*>(element);
  if (!f)
    return;

  if (on)
    m_overlay->selectFeature(f);
  else
    m_overlay->unselectFeature(f);
}

QString FeatureOverlayManager::elementDescription(GeoElement* element) const
{
  if (!element)
    return "";

  AttributeListModel* atts = element->attributes();
  if (!atts)
    return "";

  QString oid = atts->attributeValue("OID").toString();

  return QString("%1 (%2)").arg(m_overlay->name(), oid);
}

GeoElement* FeatureOverlayManager::elementAt(int elementId) const
{
  FeatureTable* tab = m_overlay->featureTable();
  if (!tab)
    return nullptr;

  QueryParameters qp;
  qp.setWhereClause(QString("\"FID\" = %1").arg(QString::number(elementId)));

  Feature* feature = nullptr;
  QEventLoop loop;
  tab->queryFeatures(qp);

  connect(tab, &FeatureTable::errorOccurred, this, [this, &loop](Error error)
  {
    qDebug() << "error:" << error.message() << error.additionalMessage();
    loop.quit();
  });

  auto connection = loop.connect(tab, &FeatureTable::queryFeaturesCompleted, this, [this, &loop, &feature](QUuid, FeatureQueryResult* featureQueryResult)
  {
    loop.quit();

    if (featureQueryResult)
      feature = featureQueryResult->iterator().next();
  });

  loop.exec();

  disconnect(connection);

  return feature;
}

qint64 FeatureOverlayManager::numberOfElements() const
{
  return m_overlay->featureTable()->numberOfFeatures();
}
