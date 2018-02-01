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

#include "AlertFilter.h"

using namespace Esri::ArcGISRuntime;

/*!
  \class AlertFilter
  \inherits QObject
  \brief Represents a filter to be applied to an alert condition data.

  This allows for certain condition data objects to be excluded - for
  example by \l AlertLevel or by specific ID.
  */

/*!
  \brief Constructor taking an optional \a parent.
 */
AlertFilter::AlertFilter(QObject* parent):
  QObject(parent)
{

}

/*!
  \brief Destructor.
 */
AlertFilter::~AlertFilter()
{

}
