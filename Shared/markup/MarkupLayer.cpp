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
#include "MarkupLayer.h"

// C++ API headers
#include "FeatureCollectionLayer.h"

// Qt headers

using namespace Esri::ArcGISRuntime;

/*
 \brief Constructor that takes a \a featureCollection and an optional \a parent.
 */
MarkupLayer::MarkupLayer(FeatureCollection* featureCollection, QObject *parent) :
  FeatureCollectionLayer(featureCollection, parent)
{
  qDebug() << "creating markup layer"; // this never prints
}

/*
 \brief Destructor
 */
MarkupLayer::~MarkupLayer()
{
}

/*
 \brief Sets the layer path to \a path.
*/
void MarkupLayer::setPath(const QString& path)
{
  m_path = path;
}

/*
 \brief Returns the layer path.
*/
QString MarkupLayer::path() const
{
  return m_path;
}


