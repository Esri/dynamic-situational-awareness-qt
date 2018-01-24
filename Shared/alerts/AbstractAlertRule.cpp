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

#include "AbstractAlertRule.h"
#include "AlertConditionData.h"

using namespace Esri::ArcGISRuntime;

AlertQuery::AlertQuery(QObject* parent):
  QObject(parent)
{

}

AlertQuery::~AlertQuery()
{

}

bool AlertQuery::active() const
{
  return m_active;
}

void AlertQuery::setActive(bool active)
{
  m_active = active;
}
