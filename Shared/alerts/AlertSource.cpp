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

#include "AlertSource.h"

using namespace Esri::ArcGISRuntime;

AlertSource::AlertSource(QObject* parent):
  QObject(parent)
{

}

AlertSource::~AlertSource()
{
  emit noLongerValid();
}
